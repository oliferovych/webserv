/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:30:19 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/11 22:45:16 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Socket.hpp"

Socket::Socket(int port) : _port(port), _fd(-1), _addr(sockaddr_in())
{
	debug_msg("Creating socket on port: " + std::to_string(port));
	setupFd();
	setupAddr();
	bindAddr();
	listenFd();
}

Socket::~Socket()
{
	debug_msg("Closing socket on port: " + std::to_string(_port));
	close(_fd);
}

void Socket::setupFd()
{
	debug_msg("Setting up socket file descriptor");
	// creating socket(socket = fd)
	if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		err_msg("Error creating socket");
		exit(1);
	}
}

void Socket::setupAddr()
{
	debug_msg("Setting up address");
	// setting up address AF_INET = IPv4, INADDR_ANY = lets the kernel choose the IP address
	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_addr.sin_port = htons(_port);
}

void Socket::bindAddr()
{
	debug_msg("Binding address to socket");
	// binding address to a socket
	if (bind(_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
	{
		err_msg("Error binding socket");
		exit(1);
	}
}

void Socket::listenFd()
{
	debug_msg("Listening to socket");
	// listening to the socket, SOMAXCONN = maximum number of connections
	if (listen(_fd, SOMAXCONN) < 0)
	{
		err_msg("Listen failed");
		exit(1);
	}
}


int Socket::getSocketFd() const
{
	return _fd;
}

int Socket::getPort() const
{
	return _port;
}


