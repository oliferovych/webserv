/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 20:14:13 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/22 20:59:52 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/Location.hpp"
#include "../../include/utils/utils.hpp"
#include <fstream>

Location::Location(std::ifstream &file, std::string const &path)
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
			_root = getSingleVarValue(line, "root");
		else if(isKeyWord(line, "index"))
			_index = getSingleVarValue(line, "index");
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
	// if(_root.empty() || _index.empty() || _allowedMethods.empty())
	// 	_valid = false;
	// else
	_valid = true;
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
