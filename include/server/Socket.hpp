/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:30:30 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/11 22:35:59 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifstream>
#include "../utils/utils.hpp"

class Socket
{
	private:
		struct sockaddr_in _addr;
		int _fd;
		int _port;
	public:
		Socket(int port);
		~Socket();

		void setupFd();
		void setupAddr();
		void bindAddr();
		void listenFd();

		int getSocketFd() const;
		int getPort() const;
}