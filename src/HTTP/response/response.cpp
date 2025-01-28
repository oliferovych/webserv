#include "../../../include/HTTP/response/Response.hpp"

Response::Response(const Request& request)
	: _result(""), _status_code(200), _request(&request), _contentDir("content"), _rootDir(getrootDir())
{
	init_mimeTypes();
}

Response::Response(void)
	: _result(""), _status_code(200), _request(nullptr), _contentDir("content"), _rootDir(getrootDir())
{
	init_mimeTypes();
}

Response::~Response(void)
{
	
}

void Response::init_mimeTypes(void)
{
	_mimeTypes = {
			{".html", "text/html"},
            {".txt", "text/plain"},
            {".css", "text/css"},
            {".js", "application/javascript"},
            {".jpg", "image/jpeg"},
            {".png", "image/png"},
			{".gif", "image/gif"},
            {".ico", "image/x-icon"}
		};
}



void Response::addHeaders(std::string category, std::vector<std::string> args)
{
	_result += category + ": ";
	for (size_t i = 0; i < args.size(); i++)
	{
		_result += args[i];
		if (i + 1 < args.size())
			_result += ", ";
	}
	_result += "\r\n";
}

void Response::build(void)
{
	std::string errorMessage = _status_code == 200 || _status_code == 201 ? "OK" : "KO";
	_result += "HTTP/1.1 " + std::to_string(_status_code);
	_result += " " + errorMessage;
	_result += "\r\n";

	addHeaders("date", {getDateHeader()});
	if (!_body.empty())
	{
		addHeaders("Content-Length", {std::to_string(_body.length())});
		addHeaders("Content-Type", {_content_type});
	}
	else
		addHeaders("Content-Length", {"0"});
	auto connection = _request->get_header("connection");
	if (!connection.empty() && connection[0] == "close")
			addHeaders("connection", {"close"});
	else
		addHeaders("connection", {"keep-alive"});
	_result += "\r\n";
	_result += _body;
}

void Response::build_err(int code, std::string message)
{
	_status_code = code;
	std::string errorMessage = message;
	_result += "HTTP/1.1 " + std::to_string(_status_code);
	_result += " " + errorMessage;
	_result += "\r\n";

	error_body(_status_code, errorMessage);

	addHeaders("Content-Length", {std::to_string(_body.length())});
	addHeaders("Content-Type", {_content_type});
	addHeaders("date", {getDateHeader()});
	addHeaders("connection", {"close"});
	_result += "\r\n";
	_result += _body;
}

void Response::doMethod(void)
{
	std::string method = _request->get_method();
	if (method == "GET")
		GET();
	else if (method == "POST")
		POST();
	else
		DELETE();
}

std::filesystem::path Response::getrootDir()
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    
    while (!std::filesystem::exists(currentPath / "Makefile"))
	{
        currentPath = currentPath.parent_path();
        if (currentPath == currentPath.root_path())
            return std::filesystem::path();
    }
	std::filesystem::path contentPath = currentPath / _contentDir;
    if (std::filesystem::exists(contentPath))
        return contentPath;
	else
		return ("");
}

void Response::error_body(int code, const std::string &errorMessage)
{
	if (code != _status_code)
    	_status_code = code;
    setBody(_rootDir / "ERROR.html");
    
    size_t pos = 0;
    while ((pos = _body.find("{{ERROR_CODE}}", pos)) != std::string::npos)
	{
        _body.replace(pos, std::string("{{ERROR_CODE}}").length(), std::to_string(code));
        pos += std::string("{{ERROR_CODE}}").length();
    }

    pos = 0;
    while ((pos = _body.find("{{ERROR_MESSAGE}}", pos)) != std::string::npos)
	{
        _body.replace(pos, std::string("{{ERROR_MESSAGE}}").length(), errorMessage);
        pos += std::string("{{ERROR_MESSAGE}}").length();
    }
}



std::string Response::getDateHeader(void)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::tm tm = *std::gmtime(&now_time_t);
    char buffer[100];

    // Format the date as "Day, DD Mon YYYY HH:MM:SS GMT"
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", &tm);

    return std::string(buffer);
}

void Response::setCode(int code)
{
	_status_code = code;
}

std::string Response::getResult() const
{
	return (_result);
}

