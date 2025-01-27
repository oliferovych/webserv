/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 15:31:35 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/27 14:37:30 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/ServerConfig.hpp"
#include "../../include/global.hpp"
#include <fstream>

ServerConfig::ServerConfig(std::string const &path) : _maxBodySize(0)
{
	_loadConfig(path);
}

ServerConfig::~ServerConfig()
{
}

bool ServerConfig::_parseServer(std::ifstream &file)
{
	std::string line;
	while(std::getline(file, line))
	{
		if(line.empty() || line[0] == '#')
			continue;
		if(isLineConsistsOnlyOf(line, "}"))
			break;

		if(isKeyWord(line, "listen"))
			_ports.push_back(std::stoi(getSingleVarValue(line, "listen")));
		else if(isKeyWord(line, "server_name"))
		{
			try{
				_serverNames = getMultipleVarValue(line, "server_name");
			}
			catch(const std::exception& e){return false;}
		}
		else if(isKeyWord(line, "root") && _root.empty())
		{
			_root = getSingleVarValue(line, "root");
			if(_root.back() != '/')
				_root += "/";
		}
		else if(isKeyWord(line, "index") && _index.empty())
		{
			_index = getSingleVarValue(line, "index");
			if(_index.front() == '/')
				_index = _index.substr(1);
		}
		else if(isKeyWord(line, "location"))
		{
			Location loc(file, getSingleVarValue(line, "location"), _root);
			if(!loc.isValid())
				return false;
			_locations.push_back(loc);
		}
		else if(isKeyWord(line, "max_body_size") && _maxBodySize == 0)
		{
			_maxBodySize = std::stoi(getSingleVarValue(line, "max_body_size"));
			if(_maxBodySize > SOMAXCONN)
			{
				err_msg("Invalid max body size");
				return false;
			}
		}
		else
		{
			err_msg("Invalid keyword in server block: " + line);
			return false;
		}
	}
	if(_ports.empty() || _serverNames.empty() || _root.empty() || _index.empty())
		return false;
	else if(!isDir(_root))
	{
		err_msg("Server's root directory does not exist");
		return false;
	}
	else if(!fileExists(_root + _index))
	{
		err_msg("Server's index file does not exist");
		return false;
	}
	return true;
}
void ServerConfig::_printOut()
{
	std::cout << "Server config:" << std::endl;
	std::cout << std::endl;
	std::cout << "Ports: ";
	for(auto port : _ports)
		std::cout << port << " ";
	std::cout << std::endl;
	std::cout << "Server names: ";
	for(auto name : _serverNames)
		std::cout << name << " ";
	std::cout << std::endl;
	std::cout << "Max body size: " << _maxBodySize << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Index: " << _index << std::endl;
	for(auto loc : _locations)
		loc.printOut(1);
	std::cout << std::endl;
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
	_printOut();
}

bool ServerConfig::isValid() const
{
	return _valid;
}
