/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:53:51 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/15 18:04:53 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../global.hpp"
#include <unordered_map>

class Client;

class Server
{
	private:
		std::unordered_map<int, Socket*> _sockets;
		std::unordered_map<int, Client*> _clients;
		SocketPoll _poll;
		void _acceptClient(int serverFd);
		bool _isServer(int fd);
		void _closeClient(int clientFd);
		void _closeServerSock(int serverFd);
	public:
		Server();
		~Server();
		void createServerSockets();
		void run();
};
