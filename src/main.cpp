/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:30:23 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/20 15:58:41 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/global.hpp"

int main(int argc, char **argv)
{
	if(argc > 2)
	{
		err_msg("Too many arguments");
		info_msg("Usage: ./webserv [config_file] or ./webserv");
		return 1;
	}
	try {
		Server server(argc == 1 ? "defconfigs/fancy/fancy.conf" : argv[1]);
		server.run();
	}
	catch (std::exception &e)
	{
		return 1;
	}

	return 0;
}
