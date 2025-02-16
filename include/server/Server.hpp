/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:53:51 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/10 22:48:31 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../global.hpp"
#include "../config/ServerConfig.hpp"
#include <unordered_map>
#include <signal.h>

class Client;

class Server
{
	private:
		std::vector<ServerConfig> _config;
		std::unordered_map<int, Socket*> _sockets;
		std::unordered_map<int, Client*> _clients;
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _sessionDB;
		SocketPoll _poll;
		static bool _running;  // Static member for signal handling
		static void _signalHandler(int signum);
		void _serverSignals();
		bool _loadConfig(std::string const &path);
		void _acceptClient(int serverFd);
		bool _isServer(int fd);
		void _closeClient(int clientFd);
		void _closeServerSock(int serverFd);
		void _shutdownServer();
		ServerConfig const &_findConfig(int port);
		ServerConfig const &_findConfig(std::string const &serverName);
	public:
		Server(std::string const &configPath);
		~Server();
		void createServerSockets();
		void run();
};
