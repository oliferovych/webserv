/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tecker <tecker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 00:01:19 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/08 17:16:20 by tecker           ###   ########.fr       */
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
		throw std::runtime_error("Unsupported CGI extension: " + extension);

	return interpreter;
}

std::string cgi_handler(const std::string &path)
{
	std::string interpreter = get_interpreter_path(path);
	int pipeFd[2];
	std::string output;
	char buffer[4096];
	
	char cwd[4096];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		throw std::runtime_error("Getcwd failed");
	std::string full_path = std::string(cwd) + path;

	if (pipe(pipeFd) == -1)
		throw std::runtime_error("Pipe failed");
	pid_t pid = fork();
	if (pid == -1)
	{
		close(pipeFd[0]);
		close(pipeFd[1]);
		throw std::runtime_error("Fork failed");
	}
	else if (pid == 0) //child
	{
		close(pipeFd[0]);
		dup2(pipeFd[1], 1);
		close(pipeFd[1]);

		char *execve_args[3];
		execve_args[0] = strdup(interpreter.c_str());
		execve_args[1] = strdup(full_path.c_str());
		execve_args[2] = NULL;
	
		char *envp[] = {NULL};

		if (execve(interpreter.c_str(), execve_args, envp) == -1)
			throw std::runtime_error("Execve failed");
		exit(1);
	}
	//parent
	close(pipeFd[1]);
	ssize_t bytesRead;
	while ((bytesRead = read(pipeFd[0], buffer, 4096)) > 0)
	{
		output.append(buffer, bytesRead);
	}
	close(pipeFd[0]);
	waitpid(pid, NULL, 0);

	return output;
}