#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <iomanip>

#include "../../utils/utils.hpp"
#include "../../config/ServerConfig.hpp"

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

		std::unordered_map<std::string, std::string> query_vars;

		long	content_length;
		ParseState state;

		std::vector<ServerConfig> &configVec;

		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &_sessionDB;
		std::string &_sessionID;


		void parse_request_line();
		void parse_headers();
		void parse_body();
		void parse_chunked_body();

		void validateRequestLine();
		void handle_absolute_path();
		void validateHeaders();

		ServerConfig *_findConfig(std::string &serverName);
		void parse_query_string(std::string str, size_t query_pos);




	public:
		Request(std::vector<ServerConfig> &conf, std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &sessionDB, std::string &sessionID);
		~Request();

		ServerConfig *config;

		void parse();
		void updateBuffer(const std::vector<char>& new_buffer);
		void reset();

		bool is_complete() const;

		// size_t get_content_length() const;
		const std::string &get_method() const;
		const std::string &get_path() const;
		const std::string &get_version() const;
		const std::vector<char> &get_body() const;
		const std::vector<std::string> get_header(const std::string& key) const;
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &getSessionDB(void);
		const std::string &get_sessionID() const;

		void setPath(std::string str);

		void debug_print() const;
		void debug_state() const;
};



#endif
