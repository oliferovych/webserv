/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 20:04:52 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/05 12:42:42 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include "../utils/utils.hpp"
#include <unordered_map>

class Location
{
	private:
		std::string _path;
		std::string _root;
		std::string _index;
		std::vector<std::string> _allowedMethods;
		std::unordered_map<int, std::string> _errorPages;
		std::string _uploadDir;
		std::string _autoindex;
		bool _valid;
		bool _checkLocation();
		void _resolvePathVars(std::string const &servRoot);
	public:
		Location(std::ifstream &file, std::string const &path, std::string const &servRoot);
		~Location();

		bool isValid() const { return _valid; }

		const std::string &getPath() const { return _path; }
		const std::string &getRoot() const { return _root; }
		const std::string &getIndex() const { return _index; }
		const std::string &getUploadDir() const { return _uploadDir; }
		const std::vector<std::string> &getAllowedMethods() const { return _allowedMethods; }
		std::string getErrorPage(int code);

		void printOut(int indent) const;
};
