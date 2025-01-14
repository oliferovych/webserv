/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:01:00 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/14 16:27:04 by dolifero         ###   ########.fr       */
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
	Socket *s1 = new Socket(8080);
	Socket *s2 = new Socket(8081);

	_sockets.insert(std::make_pair(s1->getSocketFd(), s1));
	_sockets.insert(std::make_pair(s2->getSocketFd(), s2));

	_poll.addFd(s1->getSocketFd());
	_poll.addFd(s2->getSocketFd());
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
		Client *clientSocket = new Client(client_fd, client_address);
		try{
			_clients.insert(std::make_pair(client_fd, clientSocket));
			_poll.addFd(client_fd);
		}catch(...){
			delete clientSocket;
			throw;
		}
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

void Server::run()
{
	createServerSockets();
	debug_msg("Poll size: " + std::to_string(_poll.size()));
	while(1)
	{
		int events = poll(_poll.getFds().data(), _poll.getFds().size(), -1);
		if(events < 0)
			return(err_msg("Poll failed " + std::string(strerror(errno))));
		for(size_t i = 0; i < _poll.size(); i++)
		{
			if(_poll.canRead(i))
			{
				debug_msg("input");
				if(_isServer(_poll.getFd(i)))
				{
					debug_msg("Accepting client on FD " + std::to_string(_poll.getFd(i)));
					_acceptClient(_poll.getFd(i));
					break ;
				}
				else
				{
					if(send(_poll.getFd(i), response, strlen(response), 0) > 0)
						info_msg("Response sent to client on FD " + std::to_string(_poll.getFd(i)) + " from "
							+ std::string(inet_ntoa(_clients[_poll.getFd(i)]->getAddr().sin_addr)));
					else
					{
						err_msg("Send failed " + std::string(strerror(errno)));
						close(_poll.getFd(i));
						_poll.removeFd(_poll.getFd(i));
						_clients.erase(_poll.getFd(i));
					}
					break ;
				}
			}
		}
	}
}
