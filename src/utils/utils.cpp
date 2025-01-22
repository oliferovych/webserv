/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:54:17 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/22 19:50:55 by dolifero         ###   ########.fr       */
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

void ft_decode(std::string &str)
{
	std::string decoded;

    for (size_t i = 0; i < str.length(); ++i)
	{
        if (str[i] == '%' && i + 2 < str.length())
		{
            std::string hex = str.substr(i + 1, 2);
            if (std::isxdigit(hex[0]) && std::isxdigit(hex[1]))
			{
				try
				{
					char decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
					decoded.push_back(decodedChar);
                	i += 2;
				}
				catch(const std::exception& e)
				{
					throw Error(500, "decoding failed");
				}
            }
			else
                throw Error(400, "Invalid percent-encoding found: " + hex);
        }
		else if (str[i] == '+')
            decoded.push_back(' ');
        else
            decoded.push_back(str[i]);
    }
    str = decoded;
}

std::string trim(const std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, last - first + 1);
}

bool isLineConsistsOnlyOf(const std::string& line, const std::string& target)
{
	std::string trimmedLine = trim(line);
	return trimmedLine == target;
}

std::vector<std::string> splitString(const std::string &str, char delimiter)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter))
	{
		tokens.push_back(token);
	}

	return tokens;
}
