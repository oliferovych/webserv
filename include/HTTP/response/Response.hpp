/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tecker <tecker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 14:54:11 by tecker            #+#    #+#             */
/*   Updated: 2025/02/22 14:54:12 by tecker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

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
		std::filesystem::path _rootDir;
		std::filesystem::path _workingDir;
		std::string _body;
		std::string _content_type;
		std::unordered_map<std::string, std::string> _mimeTypes;
		std::string _redirect;
		std::map<std::string, std::set<std::string>> _headers;
		std::multimap<std::string, std::vector<std::string>> _multi_headers;
		std::vector<std::string> _cgiBase;

		Location *_location;
		std::string _uploadDir;

		void addHeaders(const std::string &category, const std::vector<std::string> &args);
		std::string buildHeaders(void);
		void GET(void);
		void POST(void);
		void DELETE(void);
		std::filesystem::path getrootDir();
		void setBody(std::filesystem::path path);
		void init_mimeTypes(void);
		std::string getMimeType(std::filesystem::path path);
		void error_body(int code, const std::string &errorMessage);
		std::string getDateHeader(void);
		void fileCreation(std::vector<char> &content, std::string &filename);
		std::pair<std::string, std::vector<char>> extractData();
		void populate_dropdown(void);
		void autoIndex(std::string requestPath);
		void insert_sessionData(void);
		std::string cgi_handler(const std::filesystem::path &path);
		void parseHeaders_cgi(std::string &str);
		std::vector<char*> createEnvp(const std::filesystem::path &path);
		std::string parseBody_CGI(std::string &output);
		bool isCGI(std::filesystem::path path);





	public:
		Response(Request *request);
		Response(void);
		~Response();

		void build(void);
		void build_err(int code, std::string message);
		void checkLocation(void);


		void doMethod(void);

		std::string getResult() const;
		void setCode(int code);
};



#endif