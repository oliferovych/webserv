/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 15:53:51 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/13 16:20:30 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../global.hpp"
#include <unordered_map>

class Server
{
	private:
		std::unordered_map<int, Socket> _sockets;
		std::unordered_map<int, Socket> _clients;
		SocketPoll _poll;
		void _acceptClient(int serverFd);
		bool _isServer(int fd);
	public:
		Server();
		~Server();
		void createServerSockets();
		void run();
};
