#include "../../../include/HTTP/response/Response.hpp"

Response::Response(Request *request)
	: _result(""), _status_code(200), _request(request), _rootDir(getrootDir()), _location(nullptr), _isCGI(false)
{
	init_mimeTypes();
	if (!_request->config->getRoot().empty())
		_workingDir = _rootDir / _request->config->getRoot().substr(1);
	else
		throw Error(403, "no root defined in config!"); //error code
}

Response::Response(void)
	: _result(""), _status_code(200), _request(nullptr), _rootDir(getrootDir()), _location(nullptr), _isCGI(false)
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
		{".ico", "image/x-icon"},
		{".pl", "application/x-perl"},
		{".py", "application/x-python-code"},
		{".php", "application/x-httpd-php"},
		{".sh", "application/x-sh"},
		{".rb", "application/x-ruby"}
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
	std::string errorMessage = _status_code == 200 || _status_code == 201 || _status_code == 301 ? "OK" : "KO";
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
	if (!_redirect.empty())
		addHeaders("Location", {_redirect});
	auto connection = _request->get_header("connection");
	if (!connection.empty() && connection[0] == "close")
			addHeaders("connection", {"close"});
	else
		addHeaders("connection", {"keep-alive"});
	addHeaders("Set-Cookie", {"session_id=" + _request->get_sessionID()});
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
	try
	{
		std::string method = _request->get_method();
		checkLocation();
		if (method == "GET")
			GET();
		else if (method == "POST")
			POST();
		else
			DELETE();
	}
	catch(const Error& e)
	{
		err_msg(std::string(e.what()) + " | error code: " + std::to_string(e.code()));
		error_body(e.code(), std::string(e.what()));
		return ;
	}
	
}

//todo better return on error
std::filesystem::path Response::getrootDir()
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    
    while (!std::filesystem::exists(currentPath / "Makefile"))
	{
        currentPath = currentPath.parent_path();
        if (currentPath == currentPath.root_path())
            return ("");
    }
	return (currentPath);
}

void Response::error_body(int code, const std::string &errorMessage)
{
	if (code != _status_code)
    	_status_code = code;

	if (_location && !_location->getErrorPage(code).empty())
	{
			setBody(_rootDir / _location->getErrorPage(code).substr(1));
	}
	else if (_request && !_request->config->getErrorPage(code).empty())
		setBody(_rootDir / _request->config->getErrorPage(code).substr(1));
	else
	{
		_body = std::to_string(code) + " " + errorMessage;
		_content_type = "text/plain";
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


void Response::checkLocation(void)
{
	if (!_request)
		return ;
	std::filesystem::path path(_request->get_path());
	std::filesystem::path dir;


	if (std::filesystem::is_directory(path))
		dir = path;
	else 
		dir = path.parent_path();

	_location = _request->config->getLocation(dir);
	while (!_location) 
	{
		if (dir == "/")
		{
			_location = _request->config->getLocation("/");
			break;
		}

		dir = dir.parent_path();
		_location = _request->config->getLocation(dir);
	}


	if (_location)
	{
		if (!_location->getAllowedMethods().empty())
		{
			bool flag = false;
			for (auto method : _location->getAllowedMethods())
			{
				if (method == _request->get_method())
				{
					flag = true;
					break;
				}
			}
			if (!flag)
				throw Error(403, "Method not defined in config for this location: " + _request->get_method()); //error code
		}


		if (!_location->getRoot().empty() && _location->getRoot() != _request->config->getRoot())
			_workingDir = _rootDir / _location->getRoot().substr(1);

		if (_request->get_method() != "GET")
			_uploadDir = _rootDir / _location->getUploadDir().substr(1);
		if (!_location.getCGI().empty)
		{
			std::vector<std::string> cgiBase = _location.getCGI();
			if (std::search(cgiBase.begin(), cgiBase.end(), path.extension()) != cgiBase.end())
				_isCGI = true;
		}
	}
	else
	{
		debug_msg("no location found for: " + dir.string());
		if (!_request->config.getCGI().empty)
		{
			std::vector<std::string> cgiBase = _request->config.getCGI();
			if (std::search(cgiBase.begin(), cgiBase.end(), path.extension()) != cgiBase.end())
				_isCGI = true;
		}
		if (_request->get_method() != "GET")
			throw Error(403, "it isnt possbile to POST/DELETE outside a location!");
	}


	
}

