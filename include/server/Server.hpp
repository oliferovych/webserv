/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:53:51 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/17 09:31:31 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../global.hpp"
#include <unordered_map>

#define TIMEOUT_CHECK_INTERVAL 10

class Client;

class Server
{
	private:
		std::unordered_map<int, Socket*> _sockets;
		std::unordered_map<int, Client*> _clients;
		SocketPoll _poll;
		int _pollCycleCount;
		void _acceptClient(int serverFd);
		bool _isServer(int fd);
		void _closeClient(int clientFd);
		void _closeServerSock(int serverFd);
		void	_checkTimeouts();

	public:
		Server();
		~Server();
		void createServerSockets();
		void run();
};
