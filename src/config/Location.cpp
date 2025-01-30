/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 20:14:13 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/30 11:01:08 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/Location.hpp"
#include "../../include/utils/utils.hpp"

#include <fstream>

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
			if(_root.front() == '/')
				_root = _root.substr(1);
			_root = servRoot + _root;
			if(_root.back() != '/')
				_root += "/";
		}
		else if(isKeyWord(line, "index"))
		{
			_index = getSingleVarValue(line, "index");
			if(_index.front() == '/')
				_index = _index.substr(1);
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
		else
		{
			err_msg("Invalid keyword in location block: " + line);
			_valid = false;
		}
	}
	if(_root.empty() || _index.empty() || _allowedMethods.empty())
		_valid = false;
	else if(!isDir(_root)){
		err_msg("Location's root directory does not exist");
		_valid = false;
	}
	else if(!fileExists(_root + _index))
	{
		err_msg("Location's index file does not exist");
		_valid = false;
	}
	else
		_valid = true;

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
