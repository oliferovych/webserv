/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketPoll.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 15:30:15 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/12 16:10:01 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/SocketPoll.hpp"
#include "../../include/utils/utils.hpp"
#include <unistd.h>

SocketPoll::SocketPoll() : _fds(std::vector<struct pollfd>()), _timeout(-1)
{
	debug_msg("Creating socket poll");
}

SocketPoll::~SocketPoll()
{
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if(_fds[i].fd >= 0)
			close(_fds[i].fd);
	}
	_fds.clear();
	debug_msg("Socket poll destroyed");
}

void SocketPoll::addFd(int fd)
{
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;
	_fds.push_back(pfd);
}

void SocketPoll::removeFd(int fd)
{
	for(size_t i = 0; i < _fds.size(); i++)
	{
		if(_fds[i].fd == fd)
		{
			close(_fds[i].fd);
			_fds.erase(_fds.begin() + i);
			break;
		}
	}
}

int SocketPoll::wait()
{
	return poll(_fds.data(), _fds.size(), _timeout);
}

bool SocketPoll::canRead(int index)
{
	return _fds[index].revents & POLLIN;
}

int SocketPoll::get_pending_fd()
{
	// Return first ready FD found
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (canRead(i))
			return _fds[i].fd;
	}
	return -1;
}

int SocketPoll::getFd(int index)
{
	return _fds[index].fd;
}

size_t SocketPoll::size() const
{
	return _fds.size();
}

std::vector<struct pollfd> SocketPoll::getFds()
{
	return _fds;
}