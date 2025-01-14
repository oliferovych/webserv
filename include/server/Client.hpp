/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 18:19:28 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/14 18:16:50 by tomecker         ###   ########.fr       */
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
		bool _isFdOpen;
		sockaddr_in _addr;
		Request _request;

	public:
		int handle_message();
		Client(int clientFd, sockaddr_in addr);
		~Client();
		const int &getClientFd() const { return _clientFd; }
		const sockaddr_in &getAddr() const { return _addr; }
};
