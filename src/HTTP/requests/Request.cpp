#include "../../../include/HTTP/requests/Request.hpp"
#include <algorithm>

Request::Request()
	: content_length(0), state(PARSE_REQUEST_LINE)
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
        throw Error(400, "wrong format (request line)");

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
		throw Error(400, "bad format (there is a \r\n between request-line and headers)");

	// check if whole header section is presesnt
	std::vector<char> del = {'\r', '\n', '\r', '\n'};
	auto section_end = std::search(buffer.begin(), buffer.end(), del.begin(), del.end());
    if (section_end == buffer.end())
        return ;

	//string that contains whole header section
	std::string str_buffer(buffer.begin(), section_end + 2);
	
	//parsing headers line by line
	//extracting key and val
	size_t start = 0;
	while (start < str_buffer.length())
	{
		size_t pos_line_end = str_buffer.find("\r\n", start);
		if (pos_line_end == std::string::npos)
			throw Error(400, "bad format (there is no \r\n at the end of a line in the header)");
		size_t pos_colon = str_buffer.find(":", start);
		if (pos_colon == std::string::npos)
			throw Error(400, "bad format (there is no : in a header-line)");
		std::string key = str_buffer.substr(start, pos_colon - start);
		std::string value_line = str_buffer.substr(pos_colon + 1, pos_line_end - pos_colon - 1);
		ft_tolower(key);

		//checking for multiple vals
		size_t content_start = 0;
		while (content_start < value_line.length())
		{
			size_t pos_comma = value_line.find(",", content_start);
			if (pos_comma == std::string::npos)
				pos_comma = value_line.length();

			std::string val = value_line.substr(content_start, pos_comma - content_start);
			ft_trim(val);
			headers[key].push_back(val);

			content_start = pos_comma + 1;
		}

		start = pos_line_end + 2;
	}
	// delete header section from buffer
	buffer.erase(buffer.begin(), buffer.begin() + str_buffer.length() + 2);

	//check if headers are correct
	validateHeaders();

	//check if there is a body
	std::unordered_map<std::string, std::vector<std::string>>::iterator it = headers.find("transfer-encoding");
	if (it != headers.end() && request_line.method == "POST")
	{
		if (it->second.back() == "chunked")
		{
			state = PARSE_CHUNKED_BODY;
			return ;
		}
		throw Error(501, "this transfer-encoding isn't supported by the server: " + it->second.back());
	}
	it = headers.find("content-length");
	if (it != headers.end() && content_length > 0 && request_line.method == "POST")
		state = PARSE_BODY;
	else
		state = COMPLETE;
}

void Request::parse_body(void)
{
	if (buffer.size() < content_length)
		return ;

	// copy content-length bytes from the body
	body.assign(buffer.begin(), buffer.begin() + content_length);
	buffer.erase(buffer.begin(), buffer.begin() + content_length);

	state = COMPLETE;
}

void Request::parse_chunked_body()
{
    while (!buffer.empty())
    {
        std::vector<char> del = {'\r', '\n'};
		auto it = std::search(buffer.begin(), buffer.end(), del.begin(), del.end());

        if (it == buffer.end())
            return ;

		// getting the chunk size
        std::string chunk_size_str(buffer.begin(), it);
        size_t line_length = std::distance(buffer.begin(), it) + 2;
        size_t chunk_size;
        try
		{
            chunk_size = std::stoul(chunk_size_str, nullptr, 16);
        }
		catch (...)
		{
			throw Error(500, "chunk-size parsing failed");
		}

		// chunk size = 0 if we reached the end of the body
        if (chunk_size == 0)
        {
			if (buffer.size() < line_length + 2)
				return ;
            if (buffer[line_length] != '\r' || buffer[line_length + 1] != '\n')
				throw Error(400, "body ending has wrong format");

            buffer.erase(buffer.begin(), buffer.begin() + line_length + 2);
            state = COMPLETE;
            return ;
        }
		// Ensure there is enough data in the buffer for the current chunk and its CRLF
        if (buffer.size() < line_length + chunk_size + 2)
            return ;

        // Append the current chunk's data to the body
		auto chunk_start = buffer.begin() + line_length;
        body.insert(body.end(), chunk_start, chunk_start + chunk_size);
        // delete chunk + chunk size line from buffer
		buffer.erase(buffer.begin(), chunk_start + chunk_size + 2);
    }
	state = COMPLETE;
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
	content_length = 0;
	state = PARSE_REQUEST_LINE;
}

