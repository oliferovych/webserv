/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tecker <tecker@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 18:19:28 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/21 18:24:42 by tecker           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../global.hpp"
#include "../config/ServerConfig.hpp"

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

		
		std::string _sessionID;
		
		bool _connected;
		sockaddr_in _addr;
		Request _request;
		std::string _responseStr;
		Status _state;
		std::chrono::steady_clock::time_point _lastActivityTime;
		int _request_timeout;
		int _port;
		
	
	public:
		Client(int clientFd, sockaddr_in addr, std::vector<ServerConfig> &conf, int port);
		~Client();
		
		int handle_message();
		int sendResponse(std::string response);
		void resetRequest(void);
		void changeState(Status newState);
		void changeState(int newState);
		
		const int &getClientFd() const { return _clientFd; }
		const sockaddr_in &getAddr() const { return _addr; }
		bool hasRequestTimedOut() const;
		bool hasTimedOut(double timeoutTime) const;
		std::string getSessionID(void);
		std::string getResponseStr(void) const { return _responseStr; }
		bool isConnClosed(void);
		int getState(void) { return _state; }
		int getPort(void) { return _port; }
		
};
