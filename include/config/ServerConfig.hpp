/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 15:13:49 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/22 21:01:18 by dolifero         ###   ########.fr       */
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
		bool _valid;
		void _loadConfig(std::string const &path);
		bool _parseServer(std::ifstream &file);
		void _printOut();
	public:
		bool isValid() const;
		const std::vector<int> &getPorts() const { return _ports; }
		ServerConfig(std::string const &path);
		~ServerConfig();
};
