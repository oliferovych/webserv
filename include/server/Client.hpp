/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 18:19:28 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/17 09:41:47 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../global.hpp"

#define BUFFER_SIZE 1024

#define TIMER 2

class Request;

class Client
{
	private:
		enum Status
		{
			RECEIVING,
			SENDING,
			COMPLETE
    	};
		
		int _clientFd;
		bool _connected;
		sockaddr_in _addr;
		Request _request;
		Status _state;
		std::chrono::steady_clock::time_point _lastActivityTime;
	
	public:
		int handle_message();
		Client(int clientFd, sockaddr_in addr);
		~Client();
		const int &getClientFd() const { return _clientFd; }
		const sockaddr_in &getAddr() const { return _addr; }
		bool hasTimedOut() const;

		
};
