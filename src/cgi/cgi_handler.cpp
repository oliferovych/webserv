/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tecker <tecker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 00:01:19 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/14 13:28:13 by tecker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/global.hpp"
#include <sys/wait.h>

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

std::string Response::cgi_handler(const std::filesystem::path &path)
{
	_content_type = "text/html"; //really??
	if (!std::filesystem::exists(path) || std::filesystem::is_directory(path))
		throw Error(400, "invalid path for cgi: " + path.string());

	std::string interpreter = get_interpreter_path(path);
	int pipeFd[2];
	std::string output;
	char buffer[4096];
	std::unordered_map<std::string, std::string> env;

	// std::filesystem::path script_dir = path.parent_path();
	// chdir(script_dir.c_str()); //"The CGI should be run in the correct directory for relative path file access."

	env["REQUEST_METHOD"] = _request->get_method();
	env["SCRIPT_NAME"] = path.string();
	env["PATH_INFO"] = path.string();
	env["CONTENT_LENGTH"] = _request->get_content_length();
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

		char *execve_args[3];
		execve_args[0] = strdup(interpreter.c_str()); //should get freeed later!
		execve_args[1] = strdup(path.c_str()); //"Your program should call the CGI with the file requested as first argument."
		execve_args[2] = NULL;

		execve(interpreter.c_str(), execve_args, envp.data());
		exit(1);
	}
	else//parent
	{
		close(pipeFd[1]);
		struct pollfd pollFd[1];
		pollFd[0].fd = pipeFd[0];
		pollFd[0].events = POLLIN;

		if (_request->get_method() == "POST")
		{
			const std::string body(_request->get_body().begin(), _request->get_body().end());
			// write to child
			// check if writing failed
		}

		int ret = poll(pollFd, 1, 5000);
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

	//parse output

	return output;
}
