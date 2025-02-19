/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tecker <tecker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 15:13:49 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/19 15:04:53 by tecker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unordered_map>
#include <vector>
#include <iostream>
#include "Location.hpp"

class Location;

class ServerConfig
{
	private:
		std::vector<int> _ports;
		std::string _root;
		std::string _index;
		std::pair<int, std::string> _redirect;
		std::unordered_map<int, std::string> _errorPages;
		std::vector<std::string> _serverNames;
		std::vector<Location> _locations;
		std::vector<std::string> _cgi;
		unsigned int _maxBodySize;
		unsigned int _maxConn;
		bool _valid;
		bool _parseServer(std::ifstream &file);
		bool _checkConfig();
	public:
		bool isValid() const;
		void _printOut() const;

		const std::vector<int> &getPorts() const { return _ports; }
		std::vector<std::string> getServerNames() const { return _serverNames; }
		const unsigned int &getMaxBodySize() const { return _maxBodySize; }
		const unsigned int &getMaxConn() const { return _maxConn; }
		const std::string &getRoot() const { return _root; }
		const std::string &getIndex() const { return _index; }
		const std::vector<std::string> &getCGI() const { return _cgi; }
		std::string getErrorPage(int code);
		Location *getLocation(std::string path);
		const std::pair<int, std::string> getRedirect(void) const;


		ServerConfig(std::ifstream &file);
		~ServerConfig();
};
