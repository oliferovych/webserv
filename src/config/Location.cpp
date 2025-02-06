/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 20:14:13 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/06 16:23:08 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/Location.hpp"
#include "../../include/utils/utils.hpp"
#include <algorithm>
#include <fstream>


bool Location::_checkLocation()
{
	if(!_root.empty() && !isDir(_root)){
		err_msg("Location's root directory does not exist");
		return false;
	}
	if(!_index.empty() && !fileExists(_index))
	{
		err_msg("Location's index file does not exist");
		return false;
	}
	if(!_allowedMethods.empty())
	{
		for(auto &method : _allowedMethods)
		{
			if(method != "GET" && method != "POST" && method != "DELETE")
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
			if(!fileExists(page.second))
			{
				err_msg("Error page does not exist: " + page.second);
				return false;
			}
		}
	}
	if(_autoindex != "on" && _autoindex != "off")
	{
		err_msg("Invalid autoindex value: " + _autoindex);
		return false;
	}
	if(!_uploadDir.empty() && !isDir(_uploadDir))
	{
		err_msg("Upload directory does not exist: " + _uploadDir);
		return false;
	}
	else if(!_uploadDir.empty() && (_allowedMethods.empty()
		|| (std::find(_allowedMethods.begin(), _allowedMethods.end(), "POST") == _allowedMethods.end()
		&& std::find(_allowedMethods.begin(), _allowedMethods.end(), "DELETE") == _allowedMethods.end())))
	{
		err_msg("Upload directory specified without proper methods allowed");
		return false;
	}
	else if(_uploadDir.empty() && !_allowedMethods.empty()
		&& (std::find(_allowedMethods.begin(), _allowedMethods.end(), "POST") != _allowedMethods.end()
		|| std::find(_allowedMethods.begin(), _allowedMethods.end(), "DELETE") != _allowedMethods.end()))
	{
		err_msg("Methods POST or DELETE allowed without specifying upload directory");
		return false;
	}
	return true;
}

std::string resolvePath(std::string const &root, std::string const &servRoot, std::string const &path, std::string const &destination)
{
	std::string result;
	std::string finalPath;

	if(destination.front() == '/')
		result = servRoot + '/' + path + '/' + destination;
	else
		result = root + '/' + path + '/' + destination;

	bool lastWasSlash = false;
	for (char ch : result)
	{
		if (ch == '/')
		{
			if (!lastWasSlash)
			{
				finalPath += ch;
				lastWasSlash = true;
			}
		}
		else
		{
			finalPath += ch;
			lastWasSlash = false;
		}
	}
	return finalPath;
}

void Location::_resolvePathVars(std::string const &servRoot)
{
	if(!_index.empty())
		_index = resolvePath(_root, servRoot, _path, _index);
	if(!_uploadDir.empty())
		_uploadDir = resolvePath(_root, servRoot, _path, _uploadDir);
	for(auto &err : _errorPages)
		err.second = resolvePath(_root, servRoot, _path, err.second);

}

Location::Location(std::ifstream &file, std::string const &path, std::string const &servRoot) : _path(path), _autoindex("off"), _valid(false)
{
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
			if(_root.back() != '/')
				_root += "/";
		}
		else if(isKeyWord(line, "index"))
		{
			_index = getSingleVarValue(line, "index");
		}
		else if(isKeyWord(line, "allow_methods"))
		{
			try
			{
				_allowedMethods = getMultipleVarValue(line, "allow_methods");
			}
			catch(const std::exception& e){
				err_msg("Error in location block: \""+ line + "\": " + std::string(e.what()));
				return;
			}
		}
		else if(isKeyWord(line, "autoindex"))
		{
			_autoindex = getSingleVarValue(line, "autoindex");
		}
		else if(isKeyWord(line, "upload_dir"))
		{
			_uploadDir = getSingleVarValue(line, "upload_dir");
			if (_uploadDir.back() != '/')
				_uploadDir += '/';
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
			return;
		}
	}
	if(_root.empty())
		_root = servRoot;
	else if(_root.front() != '/')
		_root = '/' + _root;
	else if(_root.front() == '/')
	{
		_root = _root.substr(1);
		_root = servRoot + _root;
	}
	_resolvePathVars(servRoot);
	_valid = _checkLocation();
	if(_valid)
		debug_msg("Location block parsed");
	else
		err_msg("Location block invalid");
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
	for(int i = 0; i < indent; i++)
		std::cout << "  ";
	std::cout << "Autoindex: " << _autoindex << std::endl;
	for(int i = 0; i < indent; i++)
		std::cout << "  ";
	std::cout << "Upload directory: " << _uploadDir << std::endl;
	std::cout << std::endl;
}

std::string Location::getErrorPage(int code)
{
	auto it = _errorPages.find(code);
	if (it != _errorPages.end())
		return (it->second);
	return ("");
}
