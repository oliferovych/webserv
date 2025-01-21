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
		const Request &_request;
		std::string _contentDir;
		std::filesystem::path _rootDir;
		std::string _body;
		std::string _content_type;
		std::unordered_map<std::string, std::string> _mimeTypes;

		void addHeaders(std::string category, std::vector<std::string> args);
		void getFile(void);
		void createFile(void);
		void deleteFile(void);
		std::filesystem::path getrootDir();
		void setBody(std::filesystem::path path);
		void init_mimeTypes(void);
		std::string getMimeType(std::string path);
		void error_body(int code, const std::string &errorMessage);
		std::string getDateHeader(void);


	public:
		Response(const Request& request);
		~Response();

		void build(void);
		void build_err(void);

		void doMethod(void);

		std::string getResult() const;
		void setCode(int code);
};



#endif