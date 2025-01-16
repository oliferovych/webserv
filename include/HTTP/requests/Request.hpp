#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <iomanip>

#include "../../utils/utils.hpp"

#define MAX_ELEMENT_SIZE 8000


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
			std::string absolute_host;
		};
		

		Request_line	request_line;
		std::unordered_map<std::string, std::vector<std::string>>	headers;
		std::vector<char>	body;

		std::vector<char> buffer;

		size_t	content_length;
		ParseState state;
	
		
		void parse_request_line();
		void parse_headers();
		void parse_body();
		void parse_chunked_body();

		void validateRequestLine();
		void handle_absolute_path();
		void validateHeaders();


	
	public:
		Request();
		~Request();
		

		void parse();
		void updateBuffer(const std::vector<char>& new_buffer);
		void reset();

		bool is_complete() const;
    	
		// size_t get_content_length() const;
		// const std::string& get_method() const;
		// const std::string& get_target() const;
		// const std::string& get_version() const;
		const std::vector<std::string> Request::get_header(const std::string& key) const;
		// const std::vector<uint8_t>& get_body() const;

		void debug_print() const;
		void debug_state() const;
};



#endif