/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 15:13:49 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/30 11:26:22 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unordered_map>
#include <vector>
#include <iostream>
#include "Location.hpp"

class ServerConfig
{
	private:
		std::vector<int> _ports;
		std::string _root;
		std::string _index;
		std::unordered_map<int, std::string> _errorPages;
		std::vector<std::string> _serverNames;
		std::vector<Location> _locations;
		unsigned int _maxBodySize;
		unsigned int _maxConn;
		bool _valid;
		bool _parseServer(std::ifstream &file);
		void _printOut();
	public:
		bool isValid() const;

		const std::vector<int> &getPorts() const { return _ports; }
		std::vector<std::string> getServerNames() const { return _serverNames; }
		const unsigned int &getMaxBodySize() const { return _maxBodySize; }
		const unsigned int &getMaxConn() const { return _maxConn; }
		const std::string &getRoot() const { return _root; }
		const std::string &getIndex() const { return _index; }
		std::string getErrorPage(int code);
		Location *getLocation(std::string path);

		ServerConfig(std::ifstream &file);
		~ServerConfig();
};
