/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 20:16:12 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/22 20:52:46 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/utils/utils.hpp"
#include <algorithm>

bool isKeyWord(std::string const &line, std::string const &keyword)
{
	size_t first = std::find_if(line.begin(), line.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}) - line.begin();

	if (first == line.size())
		return false;
	size_t last = line.find(' ', first);
	if (last == std::string::npos)
		last = line.size();
	return(line.substr(first, last - first) == keyword);
}

std::string getSingleVarValue(std::string const &line, std::string const &keyword)
{
	size_t pos = line.find(keyword);
	pos += keyword.length();
	size_t first = line.find_first_not_of(' ', pos);
	if(first == std::string::npos)
		return "";
	size_t last = line.find(';', first);
	if(last == std::string::npos)
		last = line.find('{', first) - 1;
	return line.substr(first, last - first);
}

std::vector<std::string> getMultipleVarValue(std::string const &line, std::string const &keyword)
{
	std::vector<std::string> values;
	size_t pos = line.find(keyword);
	if (pos == std::string::npos)
		throw std::runtime_error("Keyword not found");

	pos += keyword.length();
	size_t first = line.find_first_not_of(' ', pos);
	if(first == std::string::npos)
		throw std::runtime_error("No value found");

	std::string sub = line.substr(first);
	size_t last = sub.find(';');
	if(last == std::string::npos)
		throw std::runtime_error("No closing ; found");

	sub = sub.substr(0, last);

	return (splitString(sub, ' '));
}
