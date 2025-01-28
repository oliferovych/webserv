/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:53:51 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/20 15:34:58 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../global.hpp"
#include "../config/ServerConfig.hpp"
#include <unordered_map>
#include <signal.h>

#define TIMEOUT_CHECK_INTERVAL 10

class Client;

class Server
{
	private:
		ServerConfig _config;
		std::unordered_map<int, Socket*> _sockets;
		std::unordered_map<int, Client*> _clients;
		SocketPoll _poll;
		int _pollCycleCount;
		static bool _running;  // Static member for signal handling
		static void _signalHandler(int signum);
		void _serverSignals();

		void _acceptClient(int serverFd);
		bool _isServer(int fd);
		void _closeClient(int clientFd);
		void _closeServerSock(int serverFd);
		void _checkTimeouts();

		void _shutdownServer();
	public:
		Server(std::string const &configPath);
		~Server();
		void createServerSockets();
		void run();
};
