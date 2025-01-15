/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:54:17 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/15 18:42:58 by dolifero         ###   ########.fr       */
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
	std::cout << FG_RED << BOLD << "[ERROR]	" << RESET << FG_RED << timestamp() << " : " << msg << RESET << std::endl;
}

void debug_msg(const std::string &msg)
{
	std::cout << DIM << "[DEBUG] " << timestamp() << " : " << msg << RESET << std::endl;
}

void info_msg(const std::string &msg)
{
	std::cout << FG_GREEN << "[INFO]  " << timestamp() << " : " << msg << RESET << std::endl;
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
