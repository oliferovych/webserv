/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:54:17 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/15 01:58:42 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/global.hpp"

std::string timestamp()
{
	std::time_t now = std::time(nullptr);
	std::tm* ltm = std::localtime(&now);
	
	char buffer[20];
	std::strftime(buffer, sizeof(buffer), "%y-%m-%d %H:%M:%S", ltm);
	
	return std::string(buffer);
}

void err_msg(const std::string &msg)
{
	std::cout << "[ERROR] " << timestamp() << " : " << msg << std::endl;
}

void debug_msg(const std::string &msg)
{
	std::cout << "[DEBUG] " << timestamp() << " : " << msg << std::endl;
}

void info_msg(const std::string &msg)
{
	std::cout << "[INFO] " << timestamp() << " : " << msg << std::endl;
}

Error::Error(int code, const std::string& msg) 
	: _message(msg), _code(code)
{
}

const char *Error::what() const noexcept
{
	return _message.c_str();
}

int Error::code() const
{
	return (_code);
}

void ft_trim(std::string &str)
{
	int front = 0;
	int back = str.length() - 1;

	while (front <= back && std::isspace(str[front]))
		++front;
	while (back >= front && std::isspace(str[back]))
		--back;
	if (front <= back)
        str = str.substr(front, back - front + 1);
    else
        str.clear();
}

void ft_tolower(std::string &str)
{
	for (char &c : str)
		c = std::tolower(c);
}

bool ft_has_whitespace_in_str(const std::string &str)
{
	for (auto c : str)
	{
		if (std::isspace(c))
			return (true);
	}
	return false;
}