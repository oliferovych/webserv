/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:30:23 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/20 15:34:01 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/global.hpp"

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		err_msg("Wrong number of arguments");
		info_msg("Usage: ./webserv <config_file>");
		return 1;
	}
	Server server(argv[1]);

	server.run();

	return 0;
}
