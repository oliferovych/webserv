/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:01:00 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/16 21:44:24 by tomecker         ###   ########.fr       */
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
		return ;
	}

	int flags = fcntl(client_fd, F_GETFL, 0);
	if(flags < 0)
	{
		err_msg("Error getting client socket flags " + std::string(strerror(errno)));
		return ((void)close(client_fd));
	}
	debug_msg("fcntl() flags: " + std::to_string(flags));
	if(fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		err_msg("Error setting client socket to non-blocking " + std::string(strerror(errno)));
		return ((void)close(client_fd));
	}
	// TODO: understand how it works
	setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, NULL, 0);

	try
	{
		Client *clientSocket = new Client(client_fd, client_address);
		try{
			_clients.insert(std::make_pair(client_fd, clientSocket));
		}catch(...){
			delete clientSocket;
			throw;
		}
		_poll.addFd(client_fd);
		info_msg("Client connected on FD " + std::to_string(client_fd) + " from " + std::string(inet_ntoa(client_address.sin_addr)));
	} catch (std::exception &e)
	{
		err_msg("Error creating client socket " + std::string(e.what()));
		return ((void)close(client_fd));
	}
}

bool Server::_isServer(int fd)
{
	if(_sockets.find(fd) != _sockets.end())
		return true;
	return false;
}

void Server::_closeClient(int clientFd)
{
	close(clientFd);
	_poll.removeFd(clientFd);
	delete _clients[clientFd];
	_clients.erase(clientFd);
	info_msg("Client socket closed on FD " + std::to_string(clientFd));
}

void Server::_closeServerSock(int serverFd)
{
	close(serverFd);
	_poll.removeFd(serverFd);
	delete _sockets[serverFd];
	_sockets.erase(serverFd);
	info_msg("Server socket closed on FD " + std::to_string(serverFd));
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
		for(auto &pfd : _poll.getFds())
		{
			if(pfd.revents & POLLIN)
			{
				if(_isServer(pfd.fd))
				{
					debug_msg("Accepting client on port " + std::to_string(_sockets[pfd.fd]->getPort()));
					_acceptClient(pfd.fd);
					break ;
				}
				if (_clients[pfd.fd]->handle_message() < 0)
				{
					_closeClient(pfd.fd);
					break ;
				}
			}
			if(pfd.revents & (POLLHUP | POLLERR | POLLHUP | POLLNVAL))
			{
				if(!_isServer(pfd.fd))
				{
					_closeClient(pfd.fd);
					break;
				}
			}
		}
	}
}
