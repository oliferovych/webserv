/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 20:04:52 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/22 20:34:36 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include "../utils/utils.hpp"

class Location
{
	private:
		std::string _path;
		std::string _root;
		std::string _index;
		std::vector<std::string> _allowedMethods;
		bool _valid;
	public:
		Location(std::ifstream &file, std::string const &path);
		~Location();

		bool isValid() const { return _valid; }

		const std::string &getPath() const { return _path; }
		const std::string &getRoot() const { return _root; }
		const std::string &getIndex() const { return _index; }
		const std::vector<std::string> &getAllowedMethods() const { return _allowedMethods; }

		void printOut(int indent) const;
};
