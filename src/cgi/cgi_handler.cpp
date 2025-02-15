/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tecker <tecker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 00:01:19 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/15 15:36:06 by tecker           ###   ########.fr       */
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

// std::string Response::_cgiRead(int pipeFd[2])
// {
	
// }

void Response::parseHeaders_cgi(std::string &str)
{
	size_t start = 0;
	while (start < str.length())
	{
		size_t pos_line_end = str.find("\r\n", start);
		if (pos_line_end == std::string::npos)
			throw Error(400, "bad format (there is no \r\n at the end of a line in the header) (CGI)");
		size_t pos_colon = str.find(":", start);
		if (pos_colon == std::string::npos)
			throw Error(400, "bad format (there is no : in a header-line) (CGI)");
		std::string key = str.substr(start, pos_colon - start);
		std::string value_line = str.substr(pos_colon + 1, pos_line_end - pos_colon - 1);
		ft_tolower(key);

		size_t content_start = 0;
		size_t pos_comma = 0;
		std::vector<std::string> vals;
		while (content_start < value_line.length())
		{
			if (key != "set-cookie")
				pos_comma = value_line.find(",", content_start);
			else
				pos_comma = value_line.find(";", content_start);
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

std::string Response::cgi_handler(const std::filesystem::path &path)
{
	_content_type = "text/html"; //really??
	if (!std::filesystem::exists(path) || std::filesystem::is_directory(path))
		throw Error(400, "invalid path for cgi: " + path.string());

	std::string interpreter = get_interpreter_path(path);
	int pipeFd[2];
	std::string output;
	std::unordered_map<std::string, std::string> env;

	// std::filesystem::path script_dir = path.parent_path();
	// chdir(script_dir.c_str()); //"The CGI should be run in the correct directory for relative path file access."

	env["REQUEST_METHOD"] = _request->get_method();
	env["SCRIPT_NAME"] = path.string();
	env["PATH_INFO"] = path.string();
	env["CONTENT_LENGTH"] = std::to_string(_request->get_content_length());
	env["UPLOAD_DIR"] = _uploadDir.empty() ? path.parent_path().string() : _uploadDir;
	env["CONTENT_TYPE"] = _request->get_header("Content-Type")[0];
    env["QUERY_STRING"] = _request->get_query_string();
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
	// debug_msg(_uploadDir);
	// ADD MORE ENV VARIABLES

	std::vector<std::string> envStrings;
	std::vector<char*> envp;
	for (const auto& [key, value] : env) {
		envStrings.push_back(key + "=" + value);
		envp.push_back(const_cast<char*>(envStrings.back().data()));
	}
	envp.push_back(nullptr);

	if (pipe(pipeFd) == -1)
		throw Error(500, "Pipe failed");
	pid_t pid = fork();
	if (pid == -1)
	{
		close(pipeFd[0]);
		close(pipeFd[1]);
		throw Error(500, "Fork failed");
	}
	else if (pid == 0) //child
	{
		close(pipeFd[0]);
		dup2(pipeFd[1], 1);
		close(pipeFd[1]);

		std::array<char *, 3> execve_args;
		execve_args[0] = const_cast<char*>(interpreter.c_str()); // Cast to char* to remove const qualifier
		execve_args[1] = const_cast<char*>(path.c_str()); // Cast to char* to remove const qualifier
		execve_args[2] = nullptr; // Null-terminate the arguments

		execve(interpreter.c_str(), execve_args.data(), envp.data());
		exit(1);
	}
	else//parent
	{
		close(pipeFd[0]);
		
		struct pollfd pollFd[2]; //0 is for input, 1 is for output
		pollFd[0].fd = pipeFd[0];
		pollFd[0].events = POLLIN;
		
		pollFd[1].fd = pipeFd[1];
		pollFd[1].events = POLLOUT;
		
		int ret = poll(pollFd, 2, 5000);
		if (ret == -1)
		{
			close(pipeFd[0]);
			throw Error(500, "Poll failed");
		}
		else if (ret == 0)
		{
			close(pipeFd[0]);
			throw Error(500, "Poll timed out");
		}

		if(_request->get_method() == "POST")
		{
			if (pollFd[1].revents & POLLOUT)
			{
				const std::string write_body(_request->get_body().begin(), _request->get_body().end());
				size_t totalWritten = 0;
				while (totalWritten < write_body.size())
				{
				    ssize_t bytesWritten = write(pollFd[1].fd, write_body.c_str() + totalWritten, write_body.size() - totalWritten);
				    if (bytesWritten == -1)
					{
				        close(pipeFd[1]);
				        throw Error(500, "Write to CGI script failed");
				    }
				    totalWritten += bytesWritten;
				}
				
				close(pipeFd[1]);
			}
		}

		char buffer[4096];
		if (pollFd[0].revents & POLLIN)
		{
			ssize_t bytesRead;
			while ((bytesRead = read(pollFd[0].fd, buffer, sizeof(buffer))) > 0)
				output.append(buffer, bytesRead);
		}
		close(pipeFd[0]);
		
		int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            throw Error(500, "CGI script failed with status: " + std::to_string(WEXITSTATUS(status)));
	}

	size_t header_end = output.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		header_end = output.find("\r\n");
		if (header_end == std::string::npos)
			return (output);
	}
	std::string headers = output.substr(0, header_end + 1);
	std::string body = output.substr(header_end + (output[header_end + 2] == '\n' ? 2 : 4));
	parseHeaders_cgi(headers);
	//parse output

	return output;
}
