#include "../../../include/HTTP/response/Response.hpp"
#include <fstream>
#include <sstream>

std::string Response::getMimeType(std::string path)
{
	size_t dot = path.find(".");
	if (dot != std::string::npos)
	{
		std::string end = path.substr(dot, path.length() - dot);
		auto it = _mimeTypes.find(end);
		if (it != _mimeTypes.end())
			return (it->second);
	}
	// error handling?
	return ("");
}

void Response::GET(void)
{
	std::string request_path = _request.get_path();
	if (request_path.back() == '/')
		request_path += "index.html";
	request_path.erase(0, 1);
	
	std::filesystem::path path = _rootDir / request_path;
	if (_rootDir.empty() || !std::filesystem::exists(path))
	{
		err_msg("file not found at path: " + std::string(path));
		error_body(404, "couldn't find file at path: " + path.string());
		return ;
	}
	// std::cout << "path: " << path << std::endl;
	setBody(path);
}

void Response::fileCreation(std::vector<char> &content, std::string &filename)
{
	std::string request_path = _request.get_path();
	request_path.erase(0, 1);
	std::filesystem::path path = _rootDir / request_path;

	if (filename.find('/') != std::string::npos)
		throw Error(400, "Invalid filename: " + filename);

	if (request_path.back() == '/')
		throw Error(403,  "Forbidden: Cannot create directories with Post" + path.string());

	if (path.string().find(_rootDir.string()) != 0)
		throw Error(403,  "Forbidden: Cannot create directories outside /content." + path.string());
	// std::cout << "dir: " << path.string() << std::endl;

	try
    {
        if (!path.empty() && !std::filesystem::exists(path))
        {
            info_msg("Creating directory structure: " + path.string());
            if (!std::filesystem::create_directories(path))
				throw Error(500,  "Failed to create necessary directories");
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
		throw Error(500, "Failed to create directory structure");
    }
	std::filesystem::path filePath = path / filename;
	
	std::string extension = filePath.extension().string();
	if (_mimeTypes.find(extension) == _mimeTypes.end())
		throw Error(503,  "File type not allowed by server: " + extension);

	int counter = 2;
	while (std::filesystem::exists(filePath))
	{
		filePath = path / (filePath.stem().string() + "_" + std::to_string(counter) + extension);
		counter++;
	}

	std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open())
		throw Error(500, "Failed to create file at path: " + path.string());

    file.write(content.data(), content.size());
    file.close();

    if (file.fail())
		throw Error(500, "Failed to write to file: " + path.string());

    _status_code = 201;
}

std::pair<std::string, std::vector<char>> Response::extractData()
{
	std::string boundary = _request.get_header("content-type")[0];
	size_t begin = boundary.find("boundary=");
	if (begin == std::string::npos)
		throw Error(400, "can't find boundary in Content-Type");
	std::string del = "--" + boundary.substr(begin + 9);
	std::vector<char> requestBody = _request.get_body();
	std::pair<std::string, std::vector<char>> result;

	size_t start = 0;
	auto it = std::search(requestBody.begin(), requestBody.end(), del.begin(), del.end());
	while (it != requestBody.end())
	{
		start = std::distance(requestBody.begin(), it) + del.size();
		
		if (start + 2 <= requestBody.size() && std::equal(requestBody.begin() + start, requestBody.begin() + start + 2, "--"))
            break;
		
		auto partEndIt = std::search(requestBody.begin() + start, requestBody.end(), del.begin(), del.end());
        if (partEndIt == requestBody.end())
            throw Error(400, "can't find lower boundary in body");

		std::vector<char> part(requestBody.begin() + start, partEndIt);
		std::string partStr(part.begin(), part.end());

		size_t dispositionPos = partStr.find("Content-Disposition:");
        if (dispositionPos == std::string::npos)
            throw Error(400, "Missing Content-Disposition header");

        size_t filenamePos = partStr.find("filename=\"", dispositionPos);
        if (filenamePos == std::string::npos)
            throw Error(400, "Filename not found in Content-Disposition");

        filenamePos += 10;
        size_t filenameEnd = partStr.find('"', filenamePos);
        if (filenameEnd == std::string::npos)
            throw Error(400, "Invalid filename format");

        std::string filename = partStr.substr(filenamePos, filenameEnd - filenamePos);

        size_t contentStart = partStr.find("\r\n\r\n");
        if (contentStart == std::string::npos)
            throw Error(400, "Content delimiter not found after headers");

        contentStart += 4;

        std::vector<char> fileContent(part.begin() + contentStart, part.end() - 2);
        result.first = filename;
        result.second = fileContent;

		it = std::search(requestBody.begin() + start, requestBody.end(), del.begin(), del.end());
	}
	if (result.first.empty() || result.second.empty())
		throw Error(400, "there was not enough data for POST in the body");
	return result;
}

void Response::POST(void)
{
	std::pair<std::string, std::vector<char>> result;
	try
	{
		result = extractData();
		fileCreation(result.second, result.first);
	}
	catch(const Error& e)
 	{
        err_msg("POST execution failed: " + std::string(e.what()));
        error_body(e.code(), "POST execution failed: " + std::string(e.what()));
        return;
    }
}

void Response::DELETE(void)
{
	std::string request_path = _request.get_path();
	request_path.erase(0, 1);
	std::filesystem::path path = _rootDir / request_path;
   	if (!std::filesystem::exists(path))
    {
        err_msg("File not found at path: " + path.string());
        error_body(404, "File not found at: " + path.string());
        return;
    }

	try
	{
		if (!std::filesystem::remove(path))
		{
			err_msg("Failed to delete file at path: " + path.string());
			error_body(500, "Failed to delete file at: " + path.string());
		}
    }
    catch (const std::filesystem::filesystem_error &e)
	{
        err_msg("Failed to delete file");
        error_body(500, "Failed to delete file due to system error");
    }
	_status_code = 200;
}

void Response::setBody(std::filesystem::path path)
{
	std::ifstream file(path, std::ios::binary);

	if (!file.is_open())
	{
		err_msg("file not found at path: " + path.string());
		error_body(404, "failed opening file at path: " + path.string());
		return;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	_content_type = getMimeType(path.string());
	_body = buffer.str();
}
