/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:54:52 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/16 12:54:01 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <ctime>
#include <string>

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
