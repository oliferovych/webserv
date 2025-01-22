#include "../../../include/HTTP/requests/Request.hpp"
#include <array>
#include <algorithm>

void	Request::handle_absolute_path()
{
	size_t start = request_line.path.find("://");
	if (start == std::string::npos)
		return ;
	start += 3;
	size_t path = request_line.path.find("/", start);
	if (path == std::string::npos)
		// request_line.path = "/";
		path = request_line.path.length();
	size_t port = request_line.path.find(":", start);
	if (port < path)
	{
		if (port != std::string::npos)
			request_line.absolute_host = request_line.path.substr(start, port - start);
		else
			request_line.absolute_host = request_line.path.substr(start, path - start);
		if (path != request_line.path.length())
			request_line.path = request_line.path.substr(path, request_line.path.length());
		else
			request_line.path = "/";
	}
	else
		throw Error(400, "wrong format for absolut-form-request-target: " + request_line.path);
}


void Request::validateRequestLine(void)
{
	std::array<std::string, 3> validMethods = {"GET", "POST", "DELETE"};
	auto it = std::find(validMethods.begin(), validMethods.end(), request_line.method);
	if (it == validMethods.end())
		throw Error(501, "requested method isn't known by server: " + request_line.method);

	if (request_line.version != "HTTP/1.1")
		throw Error(505, "server does not support HTTP version used: " + request_line.version);

	handle_absolute_path();
	if (request_line.path.find("..") != std::string::npos)
		throw Error(403, "server doesn't allow .. in request target: " + request_line.path);
	if (request_line.path[0] != '/')
		throw Error(400, "request target must start with a /: " + request_line.path);
	// if (request_line.path.back() == '/')
	// 	throw Error(400, "request target cant end with a /: " + request_line.path);
	bool last_was_slash = false;
	for (auto c : request_line.path)
	{
		if ((c == '/' && last_was_slash) || std::isspace(c))
			throw Error(400, "forbidden characters in request target: " + std::string(1, c));
		last_was_slash = (c == '/');
	}
}

void Request::validateHeaders()
{
	if (headers.find("host") == headers.end())
		throw Error(400, "missing host header");
	if (headers["host"].size() > 1)
		throw Error(400, "multiple host headers found");
	if (!request_line.absolute_host.empty())
		headers["host"][0] = request_line.absolute_host;

	auto it = headers.begin();
	while (it != headers.end())
	{
		if (ft_has_whitespace_in_str(it->first))
			throw Error(400, "header field name contains whitespace: " + it->first);
		++it;
	}
	it = headers.find("content-length");
	if (it != headers.end())
	{
		if (headers.find("transfer-encoding") != headers.end())
			throw Error(400, "both transfer-encoding and content-length are present in the header");
		//max content-length?
		try
		{
			content_length = std::stol(it->second[0]);
		}
		catch(...)
		{
			throw Error(500, "content_length parsing failed");
		}
		
		if (content_length < 0)
			throw Error(400, "content-length is invalid: " + std::to_string(content_length));
	}
	else if (request_line.method == "POST" && headers.find("transfer-encoding") == headers.end())
			throw Error(411, "there is some content length required");
	it = headers.find("content-type");
	if ((it == headers.end() && request_line.method == "POST") || (it != headers.end() && it->second.empty() && request_line.method == "POST"))
		throw Error(400, "content-type is missing for POST request");
	if (it != headers.end() && request_line.method == "POST" && it->second[0] != "multipart/form-data")
		throw Error(501, "server only supports multipart/form-data content type. Requested content-type: " + it->second[0]);
}