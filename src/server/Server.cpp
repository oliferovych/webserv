/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tecker <tecker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:01:00 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/22 14:40:22 by tecker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Server.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <memory>
#include <csignal>
#include <fstream>

Server::Server(std::string const &configPath)
{
	if(!_loadConfig(configPath))
	{
		err_msg("Invalid server config");
		exit(1);
	}
	_serverSignals();
}

bool Server::_running = true;

bool Server::_loadConfig(std::string const &path)
{
	if(!isFiletype(".conf", path))
	{
		err_msg("Invalid config filetype");
		return false;
	}
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		err_msg("Error opening config file");
		return false;
	}
	std::string line;
	bool htmlBlock = false;
	while(std::getline(file, line))
	{
		if(line.empty() || line[0] == '#')
			continue;
		if(isLineConsistsOnlyOf(line, "http {") && !htmlBlock)
		{
			htmlBlock = true;
			continue;
		}
		if(isLineConsistsOnlyOf(line, "}"))
			break;
		if(isLineConsistsOnlyOf(line, "server {"))
		{
			ServerConfig sc(file);
			if(!sc.isValid())
				return false;
			_config.push_back(sc);
		}
		else
		{
			err_msg("Invalid keyword in http block: " + line);
			return false;
		}
	}
	return true;
}

Server::~Server()
{
	debug_msg("Server destroyed");
}

void Server::createServerSockets()
{
	for(ServerConfig const &_conf : _config)
	{
		for(int port : _conf.getPorts())
		{
			try
			{
				std::unique_ptr<Socket> s(new Socket(port, _conf.getMaxConn() / _conf.getPorts().size()));
				_sockets.insert(std::make_pair(s->getSocketFd(), s.get()));
				_poll.addFd(s->getSocketFd());
				s.release();
			}
			catch(std::exception &e)
			{
				err_msg("Error creating server socket on port " + std::to_string(port));
				err_msg(e.what());
				throw;
			}
		}
	}
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

	// int flags = fcntl(client_fd, F_GETFL, 0);
	// if(flags < 0)
	// {
	// 	err_msg("Error getting client socket flags " + std::string(strerror(errno)));
	// 	return ((void)close(client_fd));
	// }
	// if(fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) < 0)
	// {
	// 	err_msg("Error setting client socket to non-blocking " + std::string(strerror(errno)));
	// 	return ((void)close(client_fd));
	// }
	// setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, NULL, 0);

	_setSockTimeout(client_fd, 5);

	try
	{
		Client *clientSocket = new Client(client_fd, client_address, _config, _sockets[serverFd]->getPort());
		try{
			_clients.insert(std::make_pair(client_fd, clientSocket));
		}catch(...){
			delete clientSocket;
			throw;
		}
		_poll.addFd(client_fd);
		info_msg("Client connected on FD " + std::to_string(client_fd));
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
	struct linger lo;
	lo.l_onoff = 1;
	lo.l_linger = 0;

	setsockopt(serverFd, SOL_SOCKET, SO_LINGER, &lo, sizeof(lo));

	close(serverFd);
	memset(&_sockets[serverFd]->getAddr(), 0, sizeof(_sockets[serverFd]->getAddr()));
	_poll.removeFd(serverFd);
	delete _sockets[serverFd];
	_sockets.erase(serverFd);
	info_msg("Server socket closed on FD " + std::to_string(serverFd));
}

void Server::_shutdownServer()
{
	while (!_clients.empty())
	{
		auto it = _clients.begin();
		_closeClient(it->first);
	}
	while (!_sockets.empty())
	{
		auto it = _sockets.begin();
		_closeServerSock(it->first);
	}
	info_msg("Server shutdown");
}

void Server::run()
{
	createServerSockets();
	debug_msg("Poll size: " + std::to_string(_poll.size()));
	while(_running)
	{
		int events = poll(_poll.getFds().data(), _poll.size(), 5000);
		if(events < 0)
		{
			if(errno != EINTR)
				err_msg("Poll failed " + std::string(strerror(errno)));
			break ;
		}
		else if(events == 0)
		{
			for(auto &pfd : _poll.getFds())
			{
				if(!_isServer(pfd.fd))
					_closeClient(pfd.fd);
			}
			continue ;
		}
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
				else if(_clients[pfd.fd]->hasRequestTimedOut() || _clients[pfd.fd]->handle_message() < 0)
				{
					_closeClient(pfd.fd);
					break ;
				}
			}
			if(pfd.revents & POLLOUT)
			{
				if(!_isServer(pfd.fd) && _clients[pfd.fd]->getState() != 2)
				{
					if(_clients[pfd.fd]->sendResponse(_clients[pfd.fd]->getResponseStr()) < 0)
					{
						_closeClient(pfd.fd);
						break ;
					}
					if (_clients[pfd.fd]->isConnClosed())
					{
						debug_msg("Closing connectin by request...");
						_closeClient(pfd.fd);
						break;
					}
				}
			}
			if(pfd.revents & (POLLHUP | POLLERR | POLLNVAL | POLLPRI))
			{
				debug_msg("Poll fd revent hung up/invalid, closing client fd...");
				if(!_isServer(pfd.fd))
				{
					_closeClient(pfd.fd);
					break;
				}
			}
			if(!_isServer(pfd.fd))
			{
				if(_clients[pfd.fd]->hasTimedOut(10))
				{
					_closeClient(pfd.fd);
					break ;
				}
			}
		}
	}
	_shutdownServer();
}

void Server::_setSockTimeout(const int fd, int timeOutSec)
{
	timeval time;
	time.tv_sec = timeOutSec;
	time.tv_usec = 0;

	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time)) < 0)
	{
		err_msg("Error setting receive timeout: " + std::string(strerror(errno)));
	}

	if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time)) < 0)
	{
		err_msg("Error setting send timeout: " + std::string(strerror(errno)));
	}
}

void Server::_signalHandler(int sig)
{
	(void)sig;
	_running = false;
}

void Server::_serverSignals()
{
	std::signal(SIGINT, _signalHandler);
    std::signal(SIGTERM, _signalHandler);
    std::signal(SIGQUIT, _signalHandler);
	info_msg("Signal handlers set");
}

ServerConfig const &Server::_findConfig(int port)
{
	for(ServerConfig const &sc : _config)
	{
		for(int p : sc.getPorts())
		{
			if(p == port)
				return sc;
		}
	}
	throw std::runtime_error("No server config found for port " + std::to_string(port));
}


