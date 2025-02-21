/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 20:14:13 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/21 16:54:35 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/config/Location.hpp"
#include "../../include/utils/utils.hpp"
#include "../../include/global.hpp"
#include <algorithm>
#include <fstream>


bool Location::_checkLocation()
{
	if(!_root.empty() && !isDir(_root)){
		err_msg("Location's root directory does not exist");
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
	if (!_redirect.second.empty())
	{
		if (!(_redirect.first == 301 || _redirect.first == 302))
		{
			err_msg("worng redirect code in location block: " + std::to_string(_redirect.first));
			return false;
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
	if(!_uploadDir.empty() && (_allowedMethods.empty()
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

std::string resolvePath(std::string const &path)
{
	std::string finalPath;
	bool lastWasSlash = false;

	for (char ch : path)
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
	if(!_index.empty() && _index.front() == '/')
	{
		_index = servRoot + _index.substr(1);
		_index = resolvePath(_index);
	}
	if(!_uploadDir.empty())
	{
		if (_uploadDir == "/" || _uploadDir.front() != '/')		// if uploadDir is just "/" - gets taken from the location folder
			_uploadDir = _root + _path + _uploadDir;			// same if it does not begin with "/"
		else
			_uploadDir = servRoot + _path + _uploadDir;			// if begins with "/" gets taken from the root of the server
		_uploadDir = resolvePath(_uploadDir);
	}
	for(auto &err : _errorPages)
	{
		if (err.second.front() == '/')
			err.second = servRoot + err.second.substr(1);
		else
			err.second = _root + _path.substr(1) + '/' + err.second;
		err.second = resolvePath(err.second);
	}

}

Location::Location(std::ifstream &file, std::string const &path, ServerConfig const &serv) : _path(path), _autoindex("off"), _valid(false)
{
	std::string line;
	if (_path != "/" && _path.back() == '/')
		_path.pop_back();
	while(std::getline(file, line))
	{
		if(line.empty() || line[line.find_first_not_of(" \t\r")] == '#')
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
			if (_uploadDir.front() != '/')
				_uploadDir = '/' + _uploadDir;
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
		else if(isKeyWord(line, "return"))
		{
			std::vector<std::string> values;
			try
			{
				values = getMultipleVarValue(line, "return");
				if(values.size() != 2)
				{
					err_msg("Invalid error_page block: " + line);
					return ;
				}
				_redirect = std::make_pair(std::stoi(values[0]), values[1]);
			}
			catch(const std::exception& e)
			{
				err_msg("Error in error_page block: \""+ line + "\": " + std::string(e.what()));
				return ;
			}
		}
		else if(isKeyWord(line, "cgi"))
		{
			try
			{
				_cgi = getMultipleVarValue(line, "cgi");
			}
			catch(const std::exception& e){
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
		_root = serv.getRoot();
	_resolvePathVars(serv.getRoot());
	if(_index.empty())
		_index = serv.getIndex();
	if(_cgi.empty())
		_cgi = serv.getCGI();
	if (_redirect.second.empty() && !serv.getRedirect().second.empty())
		_redirect = serv.getRedirect();
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
	displayConfigLine("Location block: " + _path, indent, FG_BOLDGREEN);
	indent++;

	displayConfigLine("Root: " + _root, indent, FG_GREEN);
	if(!_index.empty())
		displayConfigLine("Index: " + _index, indent, FG_GREEN);
	if(!_allowedMethods.empty())
	{
		std::string methods = "Allowed methods: ";
		for(auto method : _allowedMethods)
			methods.append(method + " ");
		displayConfigLine(methods, indent, FG_GREEN);
	}
	displayConfigLine("Autoindex: " + _autoindex, indent, FG_GREEN);

	if(!_uploadDir.empty())
		displayConfigLine("Upload directory: " + _uploadDir, indent, FG_GREEN);
	if(!_redirect.second.empty())
		displayConfigLine("Redirect: " + std::to_string(_redirect.first) + " " + _redirect.second, indent, FG_GREEN);

	if(!_cgi.empty())
	{	
		std::string cgi = "CGI: ";
		for(auto handler : _cgi)
			cgi.append(handler + " ");
		displayConfigLine(cgi, indent, FG_GREEN);
	}

	if(!_errorPages.empty())
	{
		displayConfigLine("Error pages:", indent, FG_GREEN);
		indent++;
		for(auto err : _errorPages)
			displayConfigLine("Err_page " + std::to_string(err.first) + ": " + err.second, indent, FG_YELLOW);
	}
}

std::string Location::getErrorPage(int code)
{
	auto it = _errorPages.find(code);
	if (it != _errorPages.end())
		return (it->second);
	return ("");
}

const std::pair<int, std::string> Location::getRedirect(void) const
{
	return (_redirect);
}
