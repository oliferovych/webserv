/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:30:23 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/11 22:52:39 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/global.hpp"

int main()
{
	Socket s1(8080);

	while(1)
	{
		int client_fd = accept(s1.getSocketFd(), NULL, NULL);
		if (client_fd < 0)
		{
			err_msg("Error accepting connection");
			continue;
		}

		// Simple HTTP response
		const char* response = "HTTP/1.1 200 OK\r\n"
		                     "Content-Type: text/plain\r\n\r\n"
		                     "Hello from WebServ!\n";

		if(send(client_fd, response, strlen(response), 0))
			close(client_fd);
	}

	return 0;
}