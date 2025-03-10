/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:30:30 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/27 14:42:32 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "../utils/utils.hpp"

class Socket
{
	private:
		int _port;
		int _fd;
		struct sockaddr_in _addr;
		unsigned int _maxConn;
	public:
		Socket(int port, unsigned int maxConn);
		~Socket();

		void setupFd();
		void setupAddr();
		void bindAddr();
		void listenFd();

		int getSocketFd() const;
		int getPort() const;
		struct sockaddr_in &getAddr();
};
