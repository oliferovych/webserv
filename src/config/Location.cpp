/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 20:14:13 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/04 19:25:49 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/Location.hpp"
#include "../../include/utils/utils.hpp"

#include <fstream>

bool Location::_checkLocation(std::string const &servRoot)
{
	if(!_root.empty() && !isDir(_root)){
		err_msg("Location's root directory does not exist");
		return false;
	}
	else if(!fileExists(_root + _index) && !fileExists(servRoot + _index))
	{
		err_msg("Location's index file does not exist");
		return false;
	}

	if(!_index.empty())
	{
		if(_index.front() == '/' && !fileExists(servRoot + _index))
		{
			err_msg("Index file does not exist: " + servRoot + _index);
			return false;
		}
		else if(_index.front() != '/' && !fileExists(_root + _index))
		{
			err_msg("Index file does not exist: " + _root + _index);
			return false;
		}
	}

	if(!_allowedMethods.empty())
	{
		for(auto &method : _allowedMethods)
		{
			if(method != "GET" && method != "POST" && method != "DELETE" && method != "PUT")
			{
				err_msg("Invalid method in location block: " + method);
				return false;
			}
		}
	}
	
	if(!_errorPages.empty())
	{
		for(auto &page : _errorPages)
		{
			if(page.second.front() == '/' && !fileExists(servRoot + page.second))
			{
				err_msg("Error page does not exist: " + servRoot + page.second);
				return false;
			}
			else if(page.second.front() != '/' && !fileExists(_root + page.second))
			{
				err_msg("Error page does not exist: " + _root + page.second);
				return false;
			}
		}
	}
	return true;
}

Location::Location(std::ifstream &file, std::string const &path, std::string const &servRoot)
{
	_path = path;
	std::string line;
	while(std::getline(file, line))
	{
		if(line.empty() || line[0] == '#')
			continue;
		if(isLineConsistsOnlyOf(line, "}"))
			break;

		if(isKeyWord(line, "root"))
		{
			_root = getSingleVarValue(line, "root");
			if(_root.front() != '/')
				_root = '/' + _root;
			if(_root.back() != '/')
				_root += "/";
		}
		else if(isKeyWord(line, "index"))
		{
			_index = getSingleVarValue(line, "index");
		}
		else if(isKeyWord(line, "uploadDir"))
		{
			_uploadDir = getSingleVarValue(line, "uploadDir");
			if (_uploadDir.back() != '/')
				_uploadDir += '/';
		}
		else if(isKeyWord(line, "allow_methods"))
		{
			try
			{
				_allowedMethods = getMultipleVarValue(line, "allow_methods");
			}
			catch(const std::exception& e){
				err_msg("Error in location block: \""+ line + "\": " + std::string(e.what()));
				_valid = false;
				return;
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
					return ;
				}
				_errorPages[std::stoi(values[0])] = values[1];
			}
			catch(const std::exception& e)
			{
				err_msg("Error in error_page block: \""+ line + "\": " + std::string(e.what()));
				return ;
			}
		}
		else
		{
			err_msg("Invalid keyword in location block: " + line);
			_valid = false;
			return;
		}
	}
	
	_valid = _checkLocation(servRoot);
	if(_valid)
		debug_msg("Location block parsed");
}

Location::~Location()
{
}

void Location::printOut(int indent) const
{
	std::cout << std::endl;
	for(int i = 0; i < indent; i++)
		std::cout << "  ";
	std::cout << "Location: " << _path << std::endl;
	for(int i = 0; i < indent; i++)
		std::cout << "  ";
	std::cout << "Root: " << _root << std::endl;
	for(int i = 0; i < indent; i++)
		std::cout << "  ";
	std::cout << "Index: " << _index << std::endl;
	for(int i = 0; i < indent; i++)
		std::cout << "  ";
	std::cout << "Allowed methods: ";
	for(auto method : _allowedMethods)
		std::cout << method << " ";
	std::cout << std::endl;
}

std::string Location::getErrorPage(int code)
{
	auto it = _errorPages.find(code);
	if (it != _errorPages.end())
		return (it->second);
	return ("");
}
