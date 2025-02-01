#include "../../../include/HTTP/response/Response.hpp"

Response::Response(Request& request)
	: _result(""), _status_code(200), _request(&request), _rootDir(getrootDir()), _location(nullptr)
{
	init_mimeTypes();
	_contentDir = _request->config->getRoot();
	// std::cout << "con dir: " << _contentDir << std::endl;
	if (_contentDir[0] == '/')
    	_contentDir = _contentDir.substr(1);
	_workingDir = _rootDir / _contentDir;
	// std::cout << "active con: " << _workingDir << std::endl;
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
	try
	{
		checkLocation();
	}
	catch(const Error& e)
	{
		err_msg(std::string(e.what()) + " | error code: " + std::to_string(e.code()));
		error_body(e.code(), std::string(e.what()));
		return ;
	}
	
	std::string method = _request->get_method();
	if (method == "GET")
		GET();
	else if (method == "POST")
		POST();
	else
		DELETE();
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
		if (_location->getErrorPage(code).front() != '/')
		{
			setBody(_workingDir / _location->getErrorPage(code));
			return ;
		}
		else
		{
			std::filesystem::path p = _rootDir / _contentDir /_location->getErrorPage(code).substr(1);
			if (std::filesystem::exists(p))
				setBody(p);
			else
			{
				_body = "Error page not found at path: " + p.string();
				_body += "\nFor error: " + std::to_string(code) + " " + errorMessage;
				_content_type = "text/plain";
			}
		}
	}
	else if (!_request->config->getErrorPage(code).empty())
		setBody(_rootDir / _contentDir / _request->config->getErrorPage(code));
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
	std::filesystem::path path(_request->get_path());
	std::filesystem::path dir;


	if (std::filesystem::is_directory(path))
		dir = path;
	else 
		dir = path.parent_path();

	_location = _request->config->getLocation(dir);
	while(!_location)
	{
		if (dir.parent_path() == "/")
			break;

		dir = dir.parent_path();
		_location = _request->config->getLocation(dir);
	}


	if (dir.string() != "/" && !_location)
		debug_msg("no location found for: " + dir.string());


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
				throw Error(403, "Method not defined in config: " + _request->get_method()); //error code
		}


		if (!_location->getRoot().empty())
			_workingDir = _rootDir / _location->getRoot().substr(1) / dir.string().substr(1);
		else
			_workingDir /= dir.string().substr(1);
		std::string p = _request->get_path();
		if (dir.string() != "/")
			p.erase(0, dir.string().size());
		_request->setPath(p);
	}

	
}

