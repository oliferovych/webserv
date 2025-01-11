/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:54:17 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/11 22:28:18 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/global.hpp"

std::string timestamp()
{
	std::time_t now = std::time(nullptr);
	std::tm* ltm = std::localtime(&now);
	
	char buffer[20];
	std::strftime(buffer, sizeof(buffer), "%y-%m-%d %H %M %S", ltm);
	
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