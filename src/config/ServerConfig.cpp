/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 15:31:35 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/02 16:48:09 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/ServerConfig.hpp"
#include "../../include/global.hpp"
#include <fstream>

ServerConfig::ServerConfig(std::ifstream &file) : _maxBodySize(0), _maxConn(0)
{
	_valid = _parseServer(file);
}

ServerConfig::~ServerConfig()
{
}

bool ServerConfig::_checkConfig()
{
	if(!_errorPages.empty())
	{
		for(auto &err : _errorPages)
		{
			if(err.second.empty())
			{
				err_msg("Error page path is empty");
				return false;
			}
			if(!fileExists(_root + err.second))
			{
				err_msg("Error page does not exist: " + err.second);
				return false;
			}
		}
	}
	if(_ports.empty() || _root.empty() || _index.empty())
		return false;
	if(!isDir(_root))
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
		else if(isKeyWord(line, "error_page"))
		{
			std::vector<std::string> values;
			try
			{
				values = getMultipleVarValue(line, "error_page");
				if(values.size() != 2)
				{
					err_msg("Invalid error_page block: " + line);
					return false;
				}
				if(values[1].front() == '/')
					values[1] = values[1].substr(1);
				_errorPages[std::stoi(values[0])] = values[1];
			}
			catch(const std::exception& e)
			{
				err_msg("Error in error_page block: \""+ line + "\": " + std::string(e.what()));
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
		_maxConn = SOMAXCONN * _ports.size();
	if(_maxBodySize == 0)
		_maxBodySize = 100000;
	if(_serverNames.empty())
		_serverNames.push_back("localhost:" + std::to_string(_ports[0]));

	_printOut();
	return _checkConfig();
}

void ServerConfig::_printOut() const
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
	for(auto err : _errorPages)
		std::cout << "  err_page" << err.first << ": " << err.second << std::endl;
	std::cout << std::endl;
}

bool ServerConfig::isValid() const
{
	return _valid;
}

Location *ServerConfig::getLocation(std::string path)
{
	for (auto &lc : _locations)
	{
		if (lc.getPath() == path)
		{
			return (&lc);
		}
	}
	return (nullptr);
}

std::string ServerConfig::getErrorPage(int code)
{
	auto it = _errorPages.find(code);
	if (it != _errorPages.end())
		return (it->second);
	return ("");
}

