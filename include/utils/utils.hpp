/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:54:52 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/25 23:27:50 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <ctime>
#include <string>
#include <vector>


class Error : public std::exception
{
	private:
		std::string _message;
		int _code;

	public:
		Error(int code, const std::string& msg);
		const char* what() const noexcept;
		int code() const;
};

std::string timestamp();
void err_msg(const std::string &msg);
void debug_msg(const std::string &msg);
void info_msg(const std::string &msg);

void ft_trim(std::string &str);
void ft_tolower(std::string &str);
bool ft_has_whitespace_in_str(const std::string &str);
void ft_decode(std::string &str);

std::string trim(const std::string& str);
bool isLineConsistsOnlyOf(const std::string& line, const std::string& target);
std::vector<std::string> splitString(const std::string &str, char delimiter);

bool isKeyWord(std::string const &line, std::string const &keyword);
std::string getSingleVarValue(std::string const &line, std::string const &keyword);
std::vector<std::string> getMultipleVarValue(std::string const &line, std::string const &keyword);

bool fileExists(const std::string &path);
bool isDir(const std::string &path);