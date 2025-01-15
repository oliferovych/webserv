#include "../../include/requests/HTTPrequest.hpp"
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

int Request::parse_request_line(void)
{
	while (buffer.size() >= 2 && buffer[0] == '\r' && buffer[1] == '\n')
        buffer.erase(buffer.begin(), buffer.begin() + 2);

	// check if whole header section is presesnt
	std::vector<char> del = {'\r', '\n'};
	auto section_end = std::search(buffer.begin(), buffer.end(), del.begin(), del.end());
	if(section_end == buffer.end())
		return 0;
	//string that contains whole header section
	std::string str_buffer(buffer.begin(), section_end);

	// extract methode, Path and http version
	size_t space_1 = str_buffer.find(" ", 0);
	if (space_1 == std::string::npos)
		return (1);
	request_line.method = str_buffer.substr(0, space_1);
	size_t space_2 = str_buffer.find(" ", space_1 + 1);
	if (space_2 == std::string::npos)
		return (1);
	request_line.path = str_buffer.substr(space_1 + 1, space_2 - space_1 - 1);
	request_line.version = str_buffer.substr(space_2 + 1, str_buffer.length() - space_2 - 1);

	// delete request_line from buffer
	buffer.erase(buffer.begin(), buffer.begin() + str_buffer.length() + 2);

	state = PARSE_HEADERS;
	return (0);
}

int Request::parse_headers(void)
{
	if (buffer.size() < 2)
        return 0;
	if (buffer[0] == '\r' && buffer[1] == '\n')
		return (1);

	// check if whole header section is presesnt
	std::vector<char> del = {'\r', '\n', '\r', '\n'};
	auto section_end = std::search(buffer.begin(), buffer.end(), del.begin(), del.end());
    if(section_end == buffer.end()){
        return 0;
	}
	std::string str_buffer(buffer.begin(), section_end + 2); //string that contains whole header section

	//parsing headers line by line
	//extracting key and val
	size_t start = 0;
	while (start < str_buffer.length())
	{
		size_t pos_line_end = str_buffer.find("\r\n", start);
		if (pos_line_end == std::string::npos)
			return (1);
		size_t pos_colon = str_buffer.find(":", start);
		if (pos_colon == std::string::npos)
			return (1);
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


	std::unordered_map<std::string, std::vector<std::string>>::iterator it = headers.find("transfer-encoding");
	if (it != headers.end() && !it->second.empty())
	{
		if (it->second.back() == "chunked")
		{
			state = PARSE_CHUNKED_BODY;
			return (0);
		}
		return (1);
	}

	it = headers.find("content-length");
	if (it != headers.end() && !it->second.empty())
    {
        content_length = std::stoi(it->second[0]);
		if (content_length > 0)
			state = PARSE_BODY;
		else
			state = COMPLETE;
    }
	else
		state = COMPLETE;

	return (0);
}

int Request::parse_body(void)
{
	if (buffer.size() < content_length)
		return (0);

	body.assign(buffer.begin(), buffer.begin() + content_length);
	buffer.erase(buffer.begin(), buffer.begin() + content_length);

	state = COMPLETE;
	debug_state();
	return (0);
}

int Request::parse_chunked_body()
{
    while (!buffer.empty())
    {
        std::vector<char> del = {'\r', '\n'};
		std::vector<char>::iterator it = std::search(buffer.begin(), buffer.end(), del.begin(), del.end());

        if (it == buffer.end())
            return 0;

        std::string chunk_size_str(buffer.begin(), it);
        size_t line_length = std::distance(buffer.begin(), it) + 2;

        size_t chunk_size;
        try
		{
            chunk_size = std::stoul(chunk_size_str, nullptr, 16);
        }
		catch (...)
		{
            return 1;
		}

        // std::cout << "chunk size: " << chunk_size << std::endl;
        if (chunk_size == 0)
        {
			if (buffer.size() < line_length + 2)
				return 0;

            if (buffer[line_length] != '\r' || buffer[line_length + 1] != '\n')
                return 1;

            buffer.erase(buffer.begin(), buffer.begin() + line_length + 2);
            state = COMPLETE;
            return 0;
        }

        if (buffer.size() < line_length + chunk_size + 2)
            return 0;

        std::vector<char>::iterator chunk_start = buffer.begin() + line_length;

        body.insert(body.end(), chunk_start, chunk_start + chunk_size);
        buffer.erase(buffer.begin(), chunk_start + chunk_size + 2);
    }

	state = COMPLETE;
    return 0;
}

bool Request::is_complete() const
{
	return (state == COMPLETE);
}


void Request::updateBuffer(const std::vector<char>& new_buffer)
{
	buffer.insert(buffer.end(), new_buffer.begin(), new_buffer.end());
}

HTTPException::HTTPException(int code, const std::string& msg)
	: _message(msg), _code(code)
{
}

const char *HTTPException::what() const noexcept
{
	return _message.c_str();
}

int HTTPException::code() const
{
	return (_code);
}

void Request::reset()
{
	request_line.method.clear();
	request_line.path.clear();
	request_line.version.clear();
	headers.clear();
	body.clear();
	buffer.clear();
	content_length = 0;
	state = PARSE_REQUEST_LINE;
}

