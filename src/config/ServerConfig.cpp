/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 15:31:35 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/27 16:53:38 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/ServerConfig.hpp"
#include "../../include/global.hpp"
#include <fstream>

ServerConfig::ServerConfig(std::string const &path) : _maxBodySize(0), _maxConn(0)
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
		}
		else if(isKeyWord(line, "max_clients") && _maxConn == 0)
		{
			_maxConn = std::stoi(getSingleVarValue(line, "max_clients"));
			if(_maxConn < 1 || _maxConn > SOMAXCONN)
			{
				err_msg("Invalid max_clients value: " + line);
				return false;
			}
		}
		else
		{
			err_msg("Invalid keyword in server block: " + line);
			return false;
		}
	}
	if(_maxConn == 0)
		_maxConn = SOMAXCONN;
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
	std::cout << "Max clients: " << _maxConn << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Index: " << _index << std::endl;
	for(auto loc : _locations)
		loc.printOut(1);
	std::cout << std::endl;
}

void ServerConfig::_loadConfig(std::string const &path)
{
	if(!isFiletype(".conf", path))
	{
		err_msg("Invalid config filetype");
		return ;
	}
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		err_msg("Error opening config file");
		return ;
	}
	std::string line;
	bool htmlBlock = false;
	while(std::getline(file, line))
	{
		if(line.empty() || line[0] == '#')
			continue;
		if(isLineConsistsOnlyOf(line, "http {") && !htmlBlock)
		{
			htmlBlock = true;
			continue;
		}
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
		else
		{
			err_msg("Invalid keyword in http block: " + line);
			_valid = 0;
			return ;
		}
	}
	_valid = 1;
	_printOut();
}

bool ServerConfig::isValid() const
{
	return _valid;
}
