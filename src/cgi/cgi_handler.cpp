/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tecker <tecker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 00:01:19 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/18 11:25:34 by tecker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/global.hpp"
#include <sys/wait.h>
#include <array>

std::string get_interpreter_path(const std::filesystem::path &path)
{
	std::string extension = path.extension().string();
	if (extension.front() != '.')
		throw Error(400, "wrong file extension for cgi: " + extension);
	std::string interpreter;

	if (extension == ".py")
		interpreter += "/usr/bin/python3";
	else if (extension == ".php")
		interpreter += "/usr/bin/php-cgi";
	else if (extension == ".pl")
		interpreter += "/usr/bin/perl";
	else if (extension == ".rb")
		interpreter += "/usr/bin/ruby";
	else if (extension == ".sh")
		interpreter += "/bin/bash";
	else
		throw Error(500, "Unsupported CGI extension: " + extension);

	return interpreter;
}


void Response::parseHeaders_cgi(std::string &str)
{
	size_t start = 0;
	while (start < str.length())
	{
		size_t pos_line_end = str.find("\r\n", start);
		if (pos_line_end == std::string::npos)
			throw Error(400, "bad format (there is no \\r\\n at the end of a line in the header) (CGI)");
		size_t pos_colon = str.find(":", start);
		if (pos_colon == std::string::npos)
			throw Error(400, "bad format (there is no : in a header-line) (CGI)");
		std::string key = str.substr(start, pos_colon - start);
		std::string value_line = str.substr(pos_colon + 1, pos_line_end - pos_colon - 1);

		size_t content_start = 0;
		size_t pos_comma = 0;
		std::vector<std::string> vals;
		while (content_start < value_line.length())
		{
			if (key != "set-cookie")
				pos_comma = value_line.find(",", content_start);
			else
				pos_comma = std::string::npos;
			if (pos_comma == std::string::npos)
				pos_comma = value_line.length();

			std::string val = value_line.substr(content_start, pos_comma - content_start);
			ft_trim(val);
			if (val.find("%") != std::string::npos)
				ft_decode(val);
			vals.push_back(val);
			content_start = pos_comma + 1;
		}
		addHeaders(key, vals);
		start = pos_line_end + 2;
	}
}

std::vector<char*> Response::createEnvp(const std::filesystem::path &path)
{
	std::unordered_map<std::string, std::string> env;
	
	env["REQUEST_METHOD"] = _request->get_method();
	env["SESSION_ID"] = _request->get_sessionID();
	env["SESSION_DB"] = "sessionDB.json";
	env["SCRIPT_NAME"] = path.string();
	env["PATH_INFO"] = path.string();
	env["UPLOAD_DIR"] = _uploadDir.empty() ? path.parent_path().string() : _uploadDir;
	env["CONTENT_LENGTH"] = std::to_string(_request->get_content_length());
    env["CONTENT_TYPE"] = _request->get_header("content-type")[0].empty() ? "" : _request->get_header("content-type")[0];
	env["QUERY_STRING"] = _request->get_query_string();
	env["SERVER_PROTOCOL"] = "HTTP/1.1";

	// std::cout << "id: " << env["SESSION_ID"] << " DB: " << env["SESSION_DB"] << std::endl;

	std::vector<char*> envp;
    for (const auto& [key, value] : env)
	{
        std::string envStr = key + "=" + value;
        char* envVar = new char[envStr.length() + 1];
        std::copy(envStr.begin(), envStr.end(), envVar);
        envVar[envStr.length()] = '\0';
        envp.push_back(envVar);
    }
    envp.push_back(nullptr);
    return envp;
}

std::string Response::parseBody_CGI(std::string &output)
{
	size_t header_end = output.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		header_end = output.find("\r\n");
		if (header_end == std::string::npos)
			return (output);
		else
			throw Error(500, "wrong header format in cgi output. Headers should end with \\r\\n\\r\\n");
	}
	std::string headers = output.substr(0, header_end + 2);
	std::string result = output.substr(header_end + 4);
	parseHeaders_cgi(headers);
	return (result);
}

void writeToChild(int *inputPipe, int *outputPipe, std::string &body, struct pollfd &pfd)
{
	pfd.fd = inputPipe[1]; //parent writing end
	pfd.events = POLLOUT;
		
	try
	{
		size_t written = 0;
		while (!body.empty() && written < body.size())
		{
			int ret = poll(&pfd, 1, 5000);
			if (ret == -1)
				throw Error(500, "poll failed while writing cgi");
			if (ret == 0)
				throw Error(500, "poll timeouted while writing cgi");
			if (!(pfd.revents & POLLOUT))
				break;
			ssize_t n = write(inputPipe[1], body.c_str() + written, body.size() - written);
			if (n > 0)
				written += n;
			else if (n < 0)
				throw Error(500, "writing to cgi script failed");
			else
				break;
		}
	}
	catch(const Error& e)
	{
		close(inputPipe[1]);
		close(outputPipe[0]);
		throw Error(500, "CGI failed: " + std::string(e.what()));
	}		
	close(inputPipe[1]); //this send EOF to child, signals end of writing to child
}

void readFromChild(int *outputPipe, struct pollfd &pfd, std::string &output)
{
	pfd.fd = outputPipe[0]; //parent reading end
	pfd.events = POLLIN;
	ssize_t bytesRead;
	std::array<char, 1024> buffer;
	try
	{
		while (true)
		{
			int ret = poll(&pfd, 1, 5000);
			if (ret == -1)
				throw Error(500, "poll failed while reading cgi");
			if (ret == 0)
				throw Error(500, "poll timeouted while reading cgi");
			if (!(pfd.revents & POLLIN))
				break;
			bytesRead = read(outputPipe[0], buffer.data(), sizeof(buffer) - 1);
			if (bytesRead < 0)
				throw Error(500, "reading from cgi script failed");
			else if (bytesRead == 0)
				break;
			output.append(buffer.data(), bytesRead);
		}
	}
	catch(const Error& e)
	{
		close(outputPipe[0]);
		throw Error(500, "CGI failed: " + std::string(e.what()));
	}
	close(outputPipe[0]);
}


std::string Response::cgi_handler(const std::filesystem::path &path)
{
	debug_msg("using CGI");
	if (!std::filesystem::exists(path) || std::filesystem::is_directory(path))
		throw Error(400, "Invalid path for CGI: " + path.string());

	std::string interpreter = get_interpreter_path(path);
	std::string output;
    std::vector<char*> envp = createEnvp(path);


	
	// Pipes:
	// [0] reads
	// [1] writes
	// inputPipe = child reads, parent writes
	// outputPipe = parent reads, child writes
	int inputPipe[2], outputPipe[2];
	if (pipe(inputPipe) == -1 || pipe(outputPipe) == -1)
		throw Error(500, "Pipe creation failed");

	pid_t pid = fork();
	if (pid == -1)
	{
		close(inputPipe[0]); close(inputPipe[1]);
		close(outputPipe[0]); close(outputPipe[1]);
		throw Error(500, "Fork failed");
	}

	if (pid == 0)
	{
		close(inputPipe[1]);   // Close unused parent write end
		close(outputPipe[0]);  // Close unused parent read end
		
		dup2(inputPipe[0], STDIN_FILENO); //reading of child becomes stdin
		dup2(outputPipe[1], STDOUT_FILENO);	//writing from child becomes stdout
		
		close(inputPipe[0]);
		close(outputPipe[1]);
		
		if (chdir(path.parent_path().string().c_str()) != 0)
			exit(1);


		std::vector<char*> args;
		args.push_back(const_cast<char*>(interpreter.c_str()));
		args.push_back(const_cast<char*>(path.c_str()));
		args.push_back(nullptr);

		execve(interpreter.c_str(), args.data(), envp.data());
		for (char* ptr : envp)
		{
			if (ptr)
				delete[] ptr;
		}
		exit(1);
	}
	else
	{
		for (char* ptr : envp)
		{
			if (ptr)
				delete[] ptr;
		}
		close(inputPipe[0]);   // close unused child reading end
		close(outputPipe[1]);  // Close unused child writing end

		std::string body(_request->get_body().begin(), _request->get_body().end());
		if (_request->get_method() != "POST")
			body.clear();
		
		struct pollfd pfd;
		writeToChild(inputPipe, outputPipe, body, pfd);
		readFromChild(outputPipe, pfd, output);
		
		int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            throw Error(500, "CGI script failed with status: " + std::to_string(WEXITSTATUS(status)));
	}
	return parseBody_CGI(output);
}
