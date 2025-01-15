/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 18:19:28 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/15 17:27:57 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../global.hpp"

#define BUFFER_SIZE 1024

class Request;

class Client
{
	private:
		int _clientFd;
		bool _connected;
		sockaddr_in _addr;
		Request _request;

	public:
		int handle_message();
		Client(int clientFd, sockaddr_in addr);
		~Client();
		const int &getClientFd() const { return _clientFd; }
		const sockaddr_in &getAddr() const { return _addr; }
};
