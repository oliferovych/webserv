/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:54:52 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/14 20:09:03 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <ctime>
#include <string>

std::string timestamp();
void err_msg(const std::string &msg);
void debug_msg(const std::string &msg);
void info_msg(const std::string &msg);

void ft_trim(std::string &str);
void ft_tolower(std::string &str);
