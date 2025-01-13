/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:01:00 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/13 16:23:00 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Server.hpp"
#include <arpa/inet.h>

Server::Server()
{
}

Server::~Server()
{
}

void Server::createServerSockets()
{
	Socket s1(8080);
	Socket s2(8081);

	_sockets.insert(std::make_pair(s1.getSocketFd(), s1));
	_sockets.insert(std::make_pair(s2.getSocketFd(), s2));

	_poll.addFd(s1.getSocketFd());
	_poll.addFd(s2.getSocketFd());
}

void Server::_acceptClient(int serverFd)
{
	sockaddr_in client_address{};
	socklen_t client_address_len = sizeof(client_address);
	int client_fd;

	if ((client_fd = accept(serverFd, reinterpret_cast<sockaddr*>(&client_address), &client_address_len)) < 0)
	{
		err_msg("Accept failed " + std::string(strerror(errno)));
		return;
	}
	// TODO: understand how it works
	setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, NULL, 0);

	try
	{
		_clients.insert(std::make_pair(client_fd, Socket(client_fd)));
		_poll.addFd(client_fd);
		info_msg("Client connected on FD " + std::to_string(client_fd) + " from " + std::string(inet_ntoa(client_address.sin_addr)));
	} catch (std::exception &e)
	{
		err_msg("Error creating client socket " + std::string(e.what()));
		close(client_fd);
	}
}

bool Server::_isServer(int fd)
{
	if(_sockets.find(fd) != _sockets.end())
		return true;
	return false;
}

void Server::run()
{
	createServerSockets();

	while(1)
	{
		if(_poll.get_pending_fd() > 0)
		{
			int pending_fd = _poll.get_pending_fd();
			while (_poll.canRead(pending_fd))
			{
				_acceptClient(pending_fd);
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
					info_msg("Response sent to FD " + std::to_string(pending_fd));
					close(client_fd);
				}
			}
		}
	}
}
