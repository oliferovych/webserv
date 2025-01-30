/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 16:01:00 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/29 16:56:30 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Server.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <memory>
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
}

void Server::createServerSockets()
{
	for(ServerConfig const &_conf : _config)
	{
		for(int port : _conf.getPorts())
		{
			std::unique_ptr<Socket> s(new Socket(port, _conf.getMaxConn()));
			try
			{
				_sockets.insert(std::make_pair(s->getSocketFd(), s.get()));
				_poll.addFd(s->getSocketFd());
				s.release();
			}
			catch(...)
			{
				err_msg("Error creating server socket on port " + std::to_string(port));
				throw;
			}
		}
	}
}

void handleMaxBodySize(int clientFd)
{
	const char* response = "HTTP/1.1 413 Payload Too Large\r\n"
                          "Content-Type: text/plain\r\n"
                          "Connection: close\r\n\r\n"
                          "Request entity too large\r\n";

	send(clientFd, response, strlen(response), 0);
	close(clientFd);
	info_msg("Client socket bounced");
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

	if((unsigned int)_clients.size() + 1 > _findConfig(_sockets[serverFd]->getPort()).getMaxConn())
	{
		err_msg("Client on port " + std::to_string(_sockets[serverFd]->getPort()) + " rejected: too many clients");
		handleMaxBodySize(client_fd);
		return ;
	}

	int flags = fcntl(client_fd, F_GETFL, 0);
	if(flags < 0)
	{
		err_msg("Error getting client socket flags " + std::string(strerror(errno)));
		return ((void)close(client_fd));
	}
	if(fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		err_msg("Error setting client socket to non-blocking " + std::string(strerror(errno)));
		return ((void)close(client_fd));
	}
	// TODO: understand how it works
	setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, NULL, 0);

	try
	{
		Client *clientSocket = new Client(client_fd, client_address, _config);
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
		int events = poll(_poll.getFds().data(), _poll.size(), -1);
		if(events < 0)
		{
			if(errno != EINTR)
				err_msg("Poll failed " + std::string(strerror(errno)));
			break ;
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
	_shutdownServer();
}

void Server::_signalHandler(int sig)
{
	(void)sig;
	_running = false;
}

void Server::_serverSignals()
{
	struct sigaction sa;
	sa.sa_handler = &_signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) < 0)
		err_msg("Error setting SIGINT handler");
	if (sigaction(SIGTERM, &sa, NULL) < 0)
		err_msg("Error setting SIGTERM handler");
	if (sigaction(SIGQUIT, &sa, NULL) < 0)
		err_msg("Error setting SIGQUIT handler");
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