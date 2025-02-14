/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 00:01:19 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/13 17:58:14 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/global.hpp"
#include <sys/wait.h>

std::string get_interpreter_path(const std::string &path)
{
	std::string extension = path.substr(path.find_last_of('.') + 1);
	std::string interpreter;

	if (extension == "py")
		interpreter += "/usr/bin/python3";
	else if (extension == "php")
		interpreter += "/usr/bin/php-cgi";
	else if (extension == "pl")
		interpreter += "/usr/bin/perl";
	else if (extension == "rb")
		interpreter += "/usr/bin/ruby";
	else if (extension == "sh")
		interpreter += "/bin/bash";
	else
		throw Error(500, "Unsupported CGI extension: " + extension);

	return interpreter;
}

std::string Response::cgi_handler(const std::string &path)
{
	_content_type = "text/html"; //really??
	if (!std::filesystem::exists(path) || std::filesystem::is_directory(path))
		throw Error(400, "invalid path for cgi: " + path);
	std::string interpreter = get_interpreter_path(path);
	int pipeFd[2];
	std::string output;
	char buffer[4096];
	std::unordered_map<std::string, std::string> env;

	env["REQUEST_METHOD"] = _request->get_method();
	env["SCRIPT_NAME"] = path;
	env["PATH_INFO"] = path;
	env["CONTENT_LENGTH"] = std::to_string(_request->get_body().size());
	env["UPLOAD_DIR"] = _uploadDir.empty() ? path.substr(0, path.find_last_of('/') + 1) : _uploadDir;
	debug_msg(_uploadDir);
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
		execve_args[0] = strdup(interpreter.c_str());
		execve_args[1] = strdup(path.c_str());
		execve_args[2] = NULL;

		if (execve(interpreter.c_str(), execve_args, envp.data()) == -1)
			throw Error(500, "Execve failed");
		exit(1);
	}
	else//parent
	{
		close(pipeFd[1]);
		struct pollfd pollFd[1];
		pollFd[0].fd = pipeFd[0];
		pollFd[0].events = POLLIN;

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
		waitpid(pid, NULL, 0);
	}

	return output;
}
