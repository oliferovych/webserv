/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:30:23 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/15 16:34:35 by dolifero         ###   ########.fr       */
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
	if(argc == 1)
	{
		Server server("defconfigs/fancy/fancy.conf");
		server.run();
	}
	else
	{
		Server server(argv[1]);
		server.run();
	}

	return 0;
}
