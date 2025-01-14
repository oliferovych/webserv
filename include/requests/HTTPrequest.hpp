#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <iomanip>




class Request
{
	private:
		enum ParseState
		{
			PARSE_REQUEST_LINE,
			PARSE_HEADERS,
			PARSE_BODY,
			PARSE_CHUNKED_BODY,
			COMPLETE
    	};

		struct Request_line
		{
			std::string method;
			std::string path;
			std::string version;
		};
		

		Request_line	request_line;
		std::unordered_map<std::string, std::vector<std::string>>	headers;
		std::vector<char>	body;

		std::vector<char> buffer;

		size_t	content_length;
		ParseState state;
	
		
		int parse_request_line();
		int parse_headers();
		int parse_body();
		int parse_chunked_body();
	
	public:
		Request();
		~Request();
		

		void parse();
		void updateBuffer(const std::vector<char>& new_buffer);

		bool is_complete() const;
    	
		size_t get_content_length() const;
		const std::string& get_method() const;
		const std::string& get_target() const;
		const std::string& get_version() const;
		std::vector<std::string> get_header(const std::string& name) const;
		const std::vector<uint8_t>& get_body() const;

		void debug_print() const;
		void debug_state() const;

		void ft_trim(std::string &str);
		void ft_tolower(std::string &str);
};

class HTTPException : public std::exception 
{
	private:
		std::string _message;
		int _code;

	public:
		HTTPException(int code, const std::string& msg);
		const char* what() const noexcept;
		int code() const;
};

#endif