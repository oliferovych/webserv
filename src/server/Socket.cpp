/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:30:19 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/27 14:43:12 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Socket.hpp"

Socket::Socket(int port, unsigned int maxConn) : _port(port), _fd(-1), _addr(sockaddr_in()), _maxConn(maxConn)
{
	debug_msg("Creating socket on port: " + std::to_string(port));
	setupFd();
	setupAddr();
	bindAddr();
	listenFd();
	info_msg("Port " + std::to_string(port) + " is now open");
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
		throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
	}
}

void Socket::setupAddr()
{
	debug_msg("Setting up address");
	int opt = 1;
	if(setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
	{
		throw std::runtime_error("Setsockopt failed: " + std::string(strerror(errno)));
	}
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
		throw std::runtime_error("Bind failed: " + std::string(strerror(errno)));
	}
}

void Socket::listenFd()
{
	debug_msg("Listening to socket");
	// listening to the socket, SOMAXCONN = maximum number of connections
	if (listen(_fd, _maxConn) < 0)
	{
		throw std::runtime_error("Listen failed: " + std::string(strerror(errno)));
	}
}

sockaddr_in &Socket::getAddr()
{
	return _addr;
}

int Socket::getSocketFd() const
{
	return _fd;
}

int Socket::getPort() const
{
	return _port;
}


