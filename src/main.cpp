/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:30:23 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/12 16:01:27 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/global.hpp"

int main()
{
	SocketPoll poll;
	Socket s1(8080);
	Socket s2(8081);

	poll.addFd(s1.getSocketFd());
	poll.addFd(s2.getSocketFd());

	while(1)
	{
		if (poll.wait() > 0)
		{
			if(poll.canRead(0))
			{
				int client_fd = accept(poll.getFd(0), NULL, NULL);
				if (client_fd > 0)
				{
					const char* response = "HTTP/1.1 200 OK\r\n"
										"Content-Type: text/html\r\n\r\n"
										"<!DOCTYPE html>\r\n"
										"<html><head><style>\r\n"
										"body {display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0;}\r\n"
										"a {font-size: 48px; text-decoration: none; transition: all 0.3s; text-decoration-line: underline;}\r\n"
										"a:hover {font-size:52px; text-decoration-line: underline;}\r\n"
										"</style></head>\r\n"
										"<body>"
										"<a href='https://www.youtube.com/watch?v=xvFZjo5PgG0'>Something cool, click on me!</a>"
										"</body></html>\r\n";
					if(send(client_fd, response, strlen(response), 0))
					{
						info_msg("Response sent to FD " + std::to_string(client_fd));
						close(client_fd);
					}
				}
			}
		}
		
	}

	return 0;
}