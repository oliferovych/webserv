#include "HTTPrequest.hpp"

Request::Request()
	: content_length(0), state(PARSE_REQUEST_LINE)
{
}

Request::~Request()
{
}

void Request::parse(void)
{
	debug_state();
	switch (state)
	{
		case PARSE_REQUEST_LINE:
			parse_request_line();
			break;
		case PARSE_HEADERS:
			parse_headers();
			break;
		case PARSE_BODY:
			parse_body();
			break;
		case PARSE_CHUNKED_BODY:
			parse_chunked_body();
			break;
		case COMPLETE:
			return ;
	}
}

int Request::parse_request_line(void)
{
	std::string str_buffer(buffer.begin(), buffer.end());
	
	int section_end = str_buffer.find("\r\n");
	if (section_end == std::string::npos)
		return (0);
	size_t space_1 = str_buffer.find(" ", 0);
	if (space_1 == std::string::npos)
		return (1);
	request_line.method = str_buffer.substr(0, space_1);
	size_t space_2 = str_buffer.find(" ", space_1 + 1);
	if (space_2 == std::string::npos)
		return (1);
	request_line.path = str_buffer.substr(space_1 + 1, space_2 - space_1 - 1);
	request_line.version = str_buffer.substr(space_2 + 1, section_end - space_2 - 1);

	buffer.erase(buffer.begin(), buffer.begin() + section_end + 2);

	state = PARSE_HEADERS;
	return (0);
}

int Request::parse_headers(void)
{
	std::string str_buffer(buffer.begin(), buffer.end());
	
	int section_end = str_buffer.find("\r\n\r\n");
	if (section_end == std::string::npos)
		return (0);
	int start = 0;
	while (start < section_end)
	{
		size_t pos_line_end = str_buffer.find("\r\n", start);
		if (pos_line_end == std::string::npos)
			return (1);
		size_t pos_colon = str_buffer.find(":", start);
		if (pos_colon == std::string::npos)
			return (1);
		std::string key = str_buffer.substr(start, pos_colon - start);

		std::string value_line = str_buffer.substr(pos_colon + 1, pos_line_end - pos_colon - 1);
		int content_start = 0;
		while (content_start < value_line.length())
		{
			size_t pos_comma = value_line.find(",", content_start);
			if (pos_comma == std::string::npos)
				pos_comma = value_line.length();

			std::string val = value_line.substr(content_start, pos_comma - content_start);
			// val.trim();
			headers[key].push_back(val);

			content_start = pos_comma + 1;
		}

		start = pos_line_end + 2;
	}
	
	buffer.erase(buffer.begin(), buffer.begin() + section_end + 4);
	
	std::unordered_map<std::string, std::vector<std::string>>::iterator it = headers.find("Transfer-Encoding");
	if (it != headers.end() && !it->second.empty())
	{
		if (it->second.back() == " chunked")
		{
			state = PARSE_CHUNKED_BODY;
			return (0);
		}
		return (1);
	}

	it = headers.find("Content-Length");
	if (it != headers.end() && !it->second.empty())
    {
        content_length = std::stoi(it->second[0]);
		if (content_length < 0)
			return (1);
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
            chunk_size = std::stol(chunk_size_str);
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


// Request req;
// std::vector<char> buffer(buffer_size);
// while (true)
// {
//     int bytes_received = recv(socket, buffer, buffer_size, 0);

//     if (bytes_received <= 0)
// 	{
// 		//error code
//         break;
//     }

// 	req.updateBuffer(std::vector<char> new_buffer(buffer.begin(), buffer.begin() + bytes_received));
// 	req.parse();

//     if (req.is_complete())
// 	{
//         response(req);
//         req.reset();    // resets existing req
//     }
// }