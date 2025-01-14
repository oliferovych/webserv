/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:01:00 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/14 20:13:59 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Server.hpp"
#include <arpa/inet.h>
#include <fcntl.h>

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

	// if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
	// 	err_msg("Failed to set client socket to non-blocking mode: " + std::string(strerror(errno)));
	// 	close(client_fd);
	// 	return;
	// }
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

void Server::run()
{
	createServerSockets();
	debug_msg("Poll size: " + std::to_string(_poll.size()));
	while(1)
	{
		int events = poll(_poll.getFds().data(), _poll.size(), -1);
		if(events < 0)
			return(err_msg("Poll failed " + std::string(strerror(errno))));
		for(size_t i = 0; i < _poll.size(); i++)
		{
			if(_poll.canRead(i))
			{
				if(_isServer(_poll.getFd(i)))
				{
					debug_msg("Accepting client on FD " + std::to_string(_poll.getFd(i)));
					_acceptClient(_poll.getFd(i));
				}
				else
				{
					if (_clients[_poll.getFd(i)]->handle_message() < 0)
					{
						close(_poll.getFd(i));
						_poll.removeFd(_poll.getFd(i));
						_clients.erase(_poll.getFd(i));
					}
				}
			}
		}
	}
}