/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tecker <tecker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 15:00:05 by tecker            #+#    #+#             */
/*   Updated: 2025/02/22 15:04:15 by tecker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../include/HTTP/requests/Request.hpp"
#include <algorithm>

Request::Request(std::vector<ServerConfig> &conf, std::string &sessionID)
	: content_length(0), state(PARSE_REQUEST_LINE), configVec(conf), _sessionID(sessionID), config(nullptr)
{
}

Request::~Request()
{
}

void Request::parse(void)
{
	if (state == PARSE_REQUEST_LINE)
		parse_request_line();
	if (state == PARSE_HEADERS)
		parse_headers();
	if (state == PARSE_BODY)
		parse_body();
	if (state == PARSE_CHUNKED_BODY)
		parse_chunked_body();
	if (state == COMPLETE)
		return ;
}

void Request::parse_request_line(void)
{
	while (buffer.size() >= 2 && buffer[0] == '\r' && buffer[1] == '\n')
        buffer.erase(buffer.begin(), buffer.begin() + 2);

	// check if whole header section is presesnt
	std::vector<char> del = {'\r', '\n'};
	auto section_end = std::search(buffer.begin(), buffer.end(), del.begin(), del.end());
  if (section_end == buffer.end())
	{
        return ;
	}
	// check size
	if (std::distance(buffer.begin(), section_end) > MAX_ELEMENT_SIZE)
	{
		throw Error(501, "request-line too large");
	}
	//string that contains whole header section
	std::string str_buffer(buffer.begin(), section_end);

	// extract methode, Path and http version
	size_t space_1 = str_buffer.find(" ", 0);
	if (space_1 == std::string::npos)
		throw Error(400, "wrong format (request line)");
	request_line.method = str_buffer.substr(0, space_1);
	size_t space_2 = str_buffer.find(" ", space_1 + 1);
	if (space_2 == std::string::npos)
		throw Error(400, "wrong format (request line)");
	request_line.path = str_buffer.substr(space_1 + 1, space_2 - space_1 - 1);
	request_line.version = str_buffer.substr(space_2 + 1, str_buffer.length() - space_2 - 1);

	if (request_line.path.find("%") != std::string::npos)
		ft_decode(request_line.path);

	// delete request_line from buffer
	buffer.erase(buffer.begin(), buffer.begin() + str_buffer.length() + 2);

	// check if all all infos are correct
	validateRequestLine();
	state = PARSE_HEADERS;
}

void Request::parse_headers(void)
{
	if (buffer.size() < 2)
        return ;
	if (buffer[0] == '\r' && buffer[1] == '\n')
		throw Error(400, "bad format (there is a \\r\\n between request-line and headers)");

	// check if whole header section is presesnt
	std::vector<char> del = {'\r', '\n', '\r', '\n'};
	auto section_end = std::search(buffer.begin(), buffer.end(), del.begin(), del.end());
  	if (section_end == buffer.end())
	{
        return ;
	}

	//check size
	if (std::distance(buffer.begin(), section_end) > MAX_ELEMENT_SIZE)
	{
		throw Error(431, "Header section too large");
	}

	//string that contains whole header section
	std::string str_buffer(buffer.begin(), section_end + 2);

	//parsing headers line by line
	//extracting key and val
	size_t start = 0;
	while (start < str_buffer.length())
	{
		size_t pos_line_end = str_buffer.find("\r\n", start);
		if (pos_line_end == std::string::npos)
			throw Error(400, "bad format (there is no \\r\\n at the end of a line in the header)");
		size_t pos_colon = str_buffer.find(":", start);
		if (pos_colon == std::string::npos)
			throw Error(400, "bad format (there is no : in a header-line)");
		std::string key = str_buffer.substr(start, pos_colon - start);
		std::string value_line = str_buffer.substr(pos_colon + 1, pos_line_end - pos_colon - 1);
		ft_tolower(key);

		//checking for multiple vals
		size_t content_start = 0;
		size_t pos_comma = 0;
		while (content_start < value_line.length())
		{
			if (key != "cookie")
				pos_comma = value_line.find(",", content_start);
			else
				pos_comma = value_line.find(";", content_start);
			if (pos_comma == std::string::npos)
				pos_comma = value_line.length();

			std::string val = value_line.substr(content_start, pos_comma - content_start);
			ft_trim(val);
			if (val.find("%") != std::string::npos)
				ft_decode(val);
			headers[key].push_back(val);

			content_start = pos_comma + 1;
		}

		start = pos_line_end + 2;
	}

	// check if path is a query string
	size_t query = request_line.path.find("?");
	if (query != std::string::npos)
	{
		query_vars = request_line.path.substr(query + 1);
		if (query_vars.find('=') == std::string::npos)
			throw Error(400, "Wrong query-string format: " + query_vars);
		request_line.path = request_line.path.substr(0, query);
	}

	// delete header section from buffer
	buffer.erase(buffer.begin(), buffer.begin() + str_buffer.length() + 2);

	//check if headers are correct
	validateHeaders();

	//check if there is a body
	std::unordered_map<std::string, std::vector<std::string>>::iterator it = headers.find("transfer-encoding");
	if (it != headers.end() && request_line.method == "POST")
	{
		if (it->second[0] == "chunked")
		{
			state = PARSE_CHUNKED_BODY;
			return ;
		}
		throw Error(501, "this transfer-encoding isn't supported by the server: " + it->second[0]);
	}
	it = headers.find("content-length");
	if (it != headers.end() && content_length > 0 && request_line.method == "POST")
		state = PARSE_BODY;
	else
		state = COMPLETE;
}

void Request::parse_body(void)
{
	if (buffer.size() < static_cast<long unsigned int> (content_length))
		return ;

	// copy content-length bytes from the body
	if (content_length > 0)
	{
		body.assign(buffer.begin(), buffer.begin() + content_length);
		buffer.erase(buffer.begin(), buffer.begin() + content_length);
	}

	state = COMPLETE;
}

void Request::parse_chunked_body()
{
	while (!buffer.empty())
    {
		printVectorEscaped(buffer);
        std::vector<char> del = {'\r', '\n', '\r', '\n'};
		auto it = std::search(buffer.begin(), buffer.end(), del.begin(), del.end());
 
		if (it != buffer.end())
		{
			if (it + 4 != buffer.end() && *(it + 4) == '\r' && *(it + 5) == '\n')
        		it += 2;
    	}
	
		//there is not a full chunk in buffer
        if (it == buffer.end())
            return ;

		// getting the chunk size
        std::vector<char> del2 = {'\r', '\n'};
		auto it2 = std::search(buffer.begin(), buffer.end(), del2.begin(), del2.end());
        std::string chunk_size_str(buffer.begin(), it2);
        size_t start = std::distance(buffer.begin(), it2) + 2;
        size_t chunk_size = 0;
		auto chunk_start = buffer.begin() + start;
        try
		{
            chunk_size = std::stoul(chunk_size_str, nullptr, 16);
		}
		catch (...)
		{
			throw Error(500, "chunk-size parsing failedd");
		}
        if (chunk_size == 0)
        {
			if (buffer.size() < start + 2)
				return ;
            if (buffer[start] != '\r' || buffer[start + 1] != '\n')
				throw Error(400, "body ending has wrong format");

            buffer.erase(buffer.begin(), buffer.begin() + start + 2);
            break ;
        }

		std::vector<char> tmp1(chunk_start, chunk_start + std::distance(chunk_start, it + 2));
		// std::cout << "tmp" << std::endl;
		// printVectorEscaped(tmp1);
		if (chunk_size > config->getMaxBodySize() - body.size())
    		throw Error(413, "request body is larger than MaxBodySize: " + std::to_string(body.size() + chunk_size));

		// Ensure there is enough data in the buffer for the current chunk and its CRLF
        if (buffer.size() < start + chunk_size + 2)
            return ;
		// Append the current chunk's data to the body
		body.insert(body.end(), tmp1.begin(), tmp1.end());
		buffer.erase(buffer.begin(), chunk_start + tmp1.size() + 2);
    }
	state = COMPLETE;
	// std::cout << "complete body" << std::endl;
	// printVectorEscaped(body);
}

bool Request::is_complete() const
{
	return (state == COMPLETE);
}


void Request::updateBuffer(const std::vector<char>& new_buffer)
{
	buffer.insert(buffer.end(), new_buffer.begin(), new_buffer.end());
}

void Request::reset()
{
	request_line.method.clear();
	request_line.path.clear();
	request_line.version.clear();
	request_line.absolute_host.clear();
	headers.clear();
	body.clear();
	buffer.clear();
	query_vars.clear();
	content_length = 0;
	state = PARSE_REQUEST_LINE;
}

const std::vector<std::string> Request::get_header(const std::string& key) const
{
	const auto it = headers.find(key);
	if (it != headers.end())
		return (it->second);
	else
		return {""};
}

const std::string &Request::get_method() const
{
	return (request_line.method);
}
const std::string &Request::get_path() const
{
	return (request_line.path);
}
const std::string &Request::get_version() const
{
	return (request_line.version);
}
const std::vector<char> &Request::get_body() const
{
	return (body);
}

ServerConfig *Request::_findConfig(std::string &serverName)
{
	for(ServerConfig &sc : configVec)
	{
		std::vector<int> ports = sc.getPorts();
		std::vector<std::string> serverNames = sc.getServerNames();
		for(std::string &sn : serverNames)
		{
			for (const int &port : ports)
			{
				if(sn == serverName || serverName == sn + ":" + std::to_string(port))
					return &sc;
			}
		}
	}
	return (nullptr);
}

void Request::setPath(std::string str)
{
	request_line.path = str;
}

const std::string &Request::get_sessionID() const
{
	return (_sessionID);
}

size_t Request::get_content_length() const
{
	return (content_length);
}

const std::string &Request::get_query_string() const
{
	return (query_vars);
}