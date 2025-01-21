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

void Response::getFile(void)
{
	std::string request_path = _request.get_path();
	request_path.erase(0, 1);
	if (request_path.back() == '/')
		request_path += "index.html";
	
	std::filesystem::path path = _rootDir / request_path;
	std::cout << "\nbef PATH: " << _rootDir.string() << std::endl;
	std::cout << "\nreq PATH: " << request_path << std::endl;
	std::cout << "\nPATH: " << path.string() << std::endl;
	if (_rootDir.empty() || !std::filesystem::exists(path))
	{
		err_msg("file not found at path: " + std::string(path));
		error_body(404, "couldn't find file at path: " + path.string());
		return ;
	}
	setBody(path);
}

void Response::createFile(void)
{
	std::string request_path = _request.get_path();
	request_path.erase(0, 1);
	std::filesystem::path path = _rootDir / request_path;
	std::filesystem::path dir = path.parent_path();

	if (request_path.back() == '/')
	{
		err_msg("can't create directories with POST: " + dir.string());
		error_body(403, "Forbidden: Cannot create directories with Post");
		return ;
	}	

	if (dir.string().find(_rootDir.string()) != 0)
	{
		err_msg("Attempt to create directories outside /content: " + dir.string());
		error_body(403, "Forbidden: Cannot create directories outside /content.");
		return ;
	}

	try
    {
        if (!dir.empty() && !std::filesystem::exists(dir))
        {
            info_msg("Creating directory structure: " + dir.string());
            if (!std::filesystem::create_directories(dir))
            {
                err_msg("Failed to create directory structure at: " + dir.string());
                error_body(500, "Failed to create necessary directories");
                return;
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        err_msg("Filesystem error while creating directories");
        error_body(500, "Failed to create directory structure");
        return;
    }

	std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        err_msg("Failed to create file at path: " + path.string());
        error_body(500, "Failed to create file at: " + path.string());
        return;
    }

    const std::vector<char> body = _request.get_body();
    file.write(body.data(), body.size());
    file.close();

    if (file.fail())
    {
        err_msg("Failed to write to file: " + path.string());
        error_body(500, "Failed to write content to file");
        return;
    }

    _status_code = 201;
}

void Response::deleteFile(void)
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