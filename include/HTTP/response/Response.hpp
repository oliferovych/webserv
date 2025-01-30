#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <iomanip>
#include <filesystem>

#include "../../utils/utils.hpp"
#include "../requests/Request.hpp"

class Response
{
	private:
		std::string _result;
		int _status_code;
		Request *_request;
		std::string _contentDir;
		std::filesystem::path _rootDir;
		std::filesystem::path _workingDir;
		std::string _body;
		std::string _content_type;
		std::unordered_map<std::string, std::string> _mimeTypes;

		Location *_location;

		void addHeaders(std::string category, std::vector<std::string> args);
		void GET(void);
		void POST(void);
		void DELETE(void);
		std::filesystem::path getrootDir();
		void setBody(std::filesystem::path path);
		void init_mimeTypes(void);
		std::string getMimeType(std::string path);
		void error_body(int code, const std::string &errorMessage);
		std::string getDateHeader(void);
		void fileCreation(std::vector<char> &content, std::string &filename);
		std::pair<std::string, std::vector<char>> extractData();
		void populate_dropdown(void);

		void checkLocation(void);


	public:
		Response(Request& request);
		Response(void);
		~Response();

		void build(void);
		void build_err(int code, std::string message);


		void doMethod(void);

		std::string getResult() const;
		void setCode(int code);
};



#endif