#include "../../../include/HTTP/response/Response.hpp"

Response::Response(Request *request)
	: _result(""), _status_code(200), _request(request), _rootDir(getrootDir()), _location(nullptr)
{
	init_mimeTypes();
	if (!_request->config->getRoot().empty())
		_workingDir = _rootDir / _request->config->getRoot().substr(1);
	else
		throw Error(403, "no root defined in config!"); //error code
}

Response::Response(void)
	: _result(""), _status_code(200), _request(nullptr), _rootDir(getrootDir()), _location(nullptr)
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

bool isMultiHeader(std::string category)
{
	//check if multiple of this header type are possible
	return (category == "Set-Cookie");
}

void Response::addHeaders(const std::string &category, const std::vector<std::string> &args)
{
	if (args.empty())
		return ;
	if (isMultiHeader(category))
		_multi_headers.emplace(category, args);
	else
	{
		for (const std::string &arg : args)
		{
			if (!arg.empty())
				_headers[category].insert(arg);
		}
	}
}

std::string Response::buildHeaders(void)
{
    for (const auto &entry : _headers)
	{
        _result += entry.first + ": ";
        bool first = true;
        for (const auto &val : entry.second)
		{
            if (!first)
                _result += ", ";
            _result += val;
            first = false;
        }
        _result += "\r\n";
    }
    for (const auto &entry : _multi_headers)
	{
        _result += entry.first + ": ";
        bool first = true;
        for (const auto &val : entry.second)
		{
            if (!first)
                _result += ", ";
            _result += val;
            first = false;
        }
        _result += "\r\n";
    }
    return _result;
}

void Response::build(void)
{
	std::string errorMessage = _status_code == 200 || _status_code == 201 || _status_code == 301 ? "OK" : "KO";
	_result.insert(0, "HTTP/1.1 " + std::to_string(_status_code) + " " + errorMessage + "\r\n");
	
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
	{
		addHeaders("connection", {"close"});
		addHeaders("Set-Cookie", {"session_id=; Max-Age=0"});
	}
	else
	{
		addHeaders("connection", {"keep-alive"});
		addHeaders("Set-Cookie", {"session_id=" + _request->get_sessionID()});
	}
	buildHeaders();
	
	_result += "\r\n";
	_result += _body;
}

void Response::build_err(int code, std::string message)
{
	_result.clear();
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
	addHeaders("Set-Cookie", {"session_id=; Max-Age=0"});
	buildHeaders();
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
		if (!_location->getCGI().empty())
			_cgiBase = _location->getCGI();
	}
	else
	{
		debug_msg("no location found for: " + dir.string());
		if (!_request->config->getCGI().empty())
			_cgiBase = _request->config->getCGI();
		if (_request->get_method() != "GET")
			throw Error(403, "it isnt possbile to POST/DELETE outside a location!");
	}


	
}

bool Response::isCGI(std::filesystem::path path)
{
	if (!_cgiBase.empty() && std::find(_cgiBase.begin(), _cgiBase.end(), path.extension().string()) != _cgiBase.end())
		return (true);
	return (false);

}

