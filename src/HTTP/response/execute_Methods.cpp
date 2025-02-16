#include "../../../include/HTTP/response/Response.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

std::string Response::getMimeType(std::string path)
{
	ft_tolower(path);
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

void Response::populate_dropdown(void)
{
	// std::cout << "gg" << std::endl;
	try
	{
        if (std::filesystem::exists(_workingDir / "uploads"))
		{
			std::string options;
			std::string str = "<option value=\"\" disabled selected>-- Select a file --</option>";
			size_t insert_pos = _body.find(str);
            if (insert_pos != std::string::npos)
			{
				insert_pos += str.length();
				for (const auto& entry : std::filesystem::directory_iterator(_workingDir / "uploads"))
				{
					if (std::filesystem::is_regular_file(entry.path()))
					{
						// std::cout << entry.path().filename() << std::endl;
						std::string filename = entry.path().filename().string();
						options += "<option value=\"" + filename + "\">" + filename + "</option>";
					}
				}
				_body.insert(insert_pos, "\n\t\t" + options);
			}
			// else
			// 	std::cout << "uu" << std::endl;

				//error handling?
		}
		// else
		// 	std::cout << "aa" << std::endl;

    }
	catch (const std::filesystem::filesystem_error& e)
	{
		err_msg("Filesystem error: " + std::string(e.what()));
    }
}

void Response::autoIndex(std::string requestPath)
{
    std::filesystem::path path = _workingDir / requestPath.substr(1);
	std::string displayPath = _location->getRoot() + requestPath.substr(1);

    if (!std::filesystem::is_directory(path))
    {
        error_body(404, "Directory does not exist. Cannot generate autoIndex. Path: " + path.string());
        return;
    }
    _body = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    _body += "<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    _body += "<title>Index of " + displayPath + "</title>\n";
    _body += "<style>\n"
             "body { font-family: 'Inter', sans-serif; background: #f5f5f7; color: #333; display: flex; "
             "justify-content: center; align-items: center; height: 100vh; margin: 0; }\n"
             ".container { max-width: 600px; width: 90%; background: white; padding: 20px; border-radius: 12px; "
             "box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1); }\n"
             "h1 { font-size: 1.5em; margin-bottom: 10px; }\n"
             "ul { list-style: none; padding: 0; margin: 0; }\n"
             "li { padding: 10px; border-radius: 8px; transition: background 0.2s, transform 0.2s; }\n"
             "li:hover { background: #f0f0f0; transform: scale(1.05); }\n"
             "a { text-decoration: none; color: #007aff; font-weight: 500; display: flex; align-items: center; }\n"
             "a:hover { color: #0056b3; }\n"
             ".icon { margin-right: 8px; font-size: 1.2em; transition: transform 0.2s; }\n"
             "li:hover .icon { transform: scale(1.2); }\n"
             "</style>\n</head>\n<body>\n";
    _body += "<div class='container'>\n<h1>📂 Index of " + displayPath + "</h1>\n<ul>\n";

    if (requestPath != "/")
        _body += "<li><a href=\"../\"><span class='icon'>⬅</span> ../ Parent Directory</a></li>\n";
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        std::string fileName = entry.path().filename().string();
        std::string filePath = requestPath + fileName;

        if (entry.is_directory())
            filePath += "/";

        std::string icon = entry.is_directory() ? "📁" : "📄";

        _body += "<li><a href=\"" + filePath + "\"><span class='icon'>" + icon + "</span>" + fileName + "</a></li>\n";
    }
    _body += "</ul>\n</div>\n</body>\n</html>";
}

void Response::insert_sessionData(void)
{
	//std::cout << _request->get_sessionID() << std::endl;
	std::string background_color;
	auto &sessionDB = _request->getSessionDB();
	auto &sessionID = _request->get_sessionID();
	auto session_it = sessionDB.find(sessionID);
	if (session_it != sessionDB.end() && session_it->second.count("background_color"))
		background_color = session_it->second["background_color"];
	else
		throw Error(500, "session not found: " + sessionID);

	size_t pos = _body.find("background-color: #333;");
	if (pos != std::string::npos)
		_body.replace(pos, 22, "background-color: " + background_color);
	else
		throw Error(500, "no background for replacement found");
}








void Response::GET(void)
{
	std::string request_path = _request->get_path();
	std::filesystem::path path;
	if (std::filesystem::is_directory(_workingDir / _request->get_path().substr(1)) && request_path.back() != '/')
	{
		_status_code = 301;
		_redirect = request_path + "/";
		return;
	}
	if (request_path.back() == '/')
	{
		if (_location)
		{
			std::string index = _location->getIndex();
			if (!index.empty() && index.substr(index.length() - 4) != "/off")
			{
				if (index.front() == '/')
					path = _rootDir / index.substr(1);
				else
					path = _workingDir / request_path.substr(1) / _location->getIndex();
				//std::cout << path.string() << std::endl;
				if (!std::filesystem::exists(path))
				{
					if (_location->getAutoIndex() == "on")
						autoIndex(request_path);
					else
					{
						err_msg("no index or autoindex found for location!");
						error_body(403, "forbidden");
					}
					return ;
				}
			}
			else if (_location->getAutoIndex() == "on")
				return (autoIndex(request_path));
			else
			{
				err_msg("no index or autoindex found for location!");
				error_body(403, "forbidden"); //right error
				return ;
			}
		}
		else if (!_request->config->getIndex().empty() && _request->config->getIndex().substr(_request->config->getIndex().length() - 4) != "/off")
			path = _workingDir / request_path.substr(1) / _request->config->getIndex();
		else
		{
			err_msg("no index or autoindex found in config!");
			error_body(403, "no index or autoindex found in config!"); //right error
			return ;
		}
	}
	else
		path = _workingDir / request_path.substr(1);
	if (!std::filesystem::exists(path))
	{
		err_msg("file not found at path: " + path.string());
		error_body(404, "couldn't find file at path: " + path.string());
		return ;
	}
	setBody(path);
}

void Response::fileCreation(std::vector<char> &content, std::string &filename)
{

	if (filename.find('/') != std::string::npos)
		throw Error(400, "Invalid filename: " + filename);

	std::string request_path = _request->get_path();
	std::filesystem::path path = _workingDir / request_path.substr(1);
	if (!std::filesystem::is_directory(path))
		throw Error(400, "Request Target needs to be a directory for POST method: " + path.string());
	std::cout << "postreq: " << request_path << " path: " << path << " uploadDir " << _uploadDir << std::endl;
	if(path.string().back() != '/')
		path += "/";
	if (path != _uploadDir)
		throw Error(403, "path is outside the uploadDir defined in the config!");
	try
	{
		if (!std::filesystem::exists(path))
		{
			std::filesystem::create_directories(path);
			debug_msg("successfully created directory structure: " + path.string());
		}
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		throw Error(500, "Failed to create directory structure");
	}
	std::filesystem::path filePath = path / filename;
	
	std::string extension = filePath.extension().string();
	ft_tolower(extension);
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
	std::string boundary = _request->get_header("content-type")[0];
	size_t begin = boundary.find("boundary=");
	if (begin == std::string::npos)
		throw Error(400, "can't find boundary in Content-Type");
	std::string del = "--" + boundary.substr(begin + 9);
	std::vector<char> requestBody = _request->get_body();
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
		for (auto c : filename)
		{
			if (std::isspace(c))
				throw Error(400, "whitespace in filename for POST isnt allowed: " + filename);
		}		

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
		if (_isCGI)
		{
			_body = cgi_handler((_workingDir / _request->get_path().substr(1)).string());
			return ;
		}
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
	std::string request_path = _request->get_path();
	std::filesystem::path path = _workingDir / request_path.substr(1);
	if (_isCGI)
	{
		_body = cgi_handler((_workingDir / _request->get_path().substr(1)).string());
		return ;
	}
	if (std::filesystem::is_directory(path))
		throw Error(403, "cant delete directories");
	if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
		throw Error(403, "cant find file for deletion at path: " + path.string());
	std::cout << "delreq: " << request_path << " path: " << path.parent_path() / "" << " uploadDir " << _uploadDir << std::endl;
	if (path.parent_path() / "" != _uploadDir)
		throw Error(403, "path is outside the uploadDir defined in the config!");
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
	if (_isCGI)
	{
		_body = cgi_handler(path.string());
		return ;
	}


	std::ifstream file(path, std::ios::binary);

	if (!file.is_open())
	{
		err_msg("file not found at path (setBody): " + path.string());
		_body = "404, File not found at path: " + path.string();
		_content_type = "text/plain";
		return;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	_content_type = getMimeType(path.string());
	_body = buffer.str();

	if (path.filename() == "template.html")
		insert_sessionData();
}
