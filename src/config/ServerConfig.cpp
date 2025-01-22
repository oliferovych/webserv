/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 15:31:35 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/22 01:38:59 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/ServerConfig.hpp"
#include "../../include/global.hpp"
#include <fstream>

ServerConfig::ServerConfig(std::string const &path)
{
	_loadConfig(path);
}

ServerConfig::~ServerConfig()
{
}

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
	return line.substr(first, last - first);
}

bool ServerConfig::_parseServer(std::ifstream &file)
{
	std::string line;
	while(std::getline(file, line))
	{
		if(line.empty() || line[0] == '#')
			continue;
		if(isLineConsistsOnlyOf(line, "}"))
		{
			debug_msg("Server block ended");
			break;
		}
		if(isKeyWord(line, "listen"))
			_ports.push_back(std::stoi(getSingleVarValue(line, "listen")));
		// else if(isKeyWord(line, "server_name"))
		// 	//parse server_name
		else if(isKeyWord(line, "root"))
			_root = getSingleVarValue(line, "root");
		else if(isKeyWord(line, "index"))
			_index = getSingleVarValue(line, "index");
		// else if(isKeyWord(line, "error_page"))
		// {
		// 	//parse error_page
		// }
		else
		{
			err_msg("Invalid keyword in server block: " + line);
			return false;
		}
	}
	return true;
}
void ServerConfig::_printOut()
{
	std::cout << "Ports: ";
	for(auto port : _ports)
		std::cout << port << " ";
	std::cout << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Index: " << _index << std::endl;
}

void ServerConfig::_loadConfig(std::string const &path)
{
	if(path.find(".conf") == std::string::npos)
	{
		err_msg("Invalid config file");
		return ;
	}
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		err_msg("Error opening config file");
		return ;
	}
	std::string line;
	while(std::getline(file, line))
	{
		if(line.empty() || line[0] == '#')
			continue;
		if(isLineConsistsOnlyOf(line, "http {"))
			continue;
		if(isLineConsistsOnlyOf(line, "}"))
			break;
		if(isLineConsistsOnlyOf(line, "server {"))
		{
			if(!_parseServer(file))
			{
				_valid = 0;
				return ;
			}
		}
	}
	_valid = 1;
	// _printOut();
}

bool ServerConfig::isValid() const
{
	return _valid;
}
