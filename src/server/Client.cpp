/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 18:23:53 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/16 01:00:54 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Client.hpp"
#include "../../include/HTTP/requests/Request.hpp"
#include <unistd.h>

Client::Client(int clientFd, sockaddr_in addr, std::vector<ServerConfig> &conf, std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &sessionDB)
	: _clientFd(clientFd), _connected(true), _addr(addr), _request(conf, sessionDB, _sessionID), _state(COMPLETE), _request_timeout(2)
{
}

Client::~Client()
{
	if (_connected)
		close(_clientFd);
}

int Client::handle_message()
{
	std::vector<char> buffer(BUFFER_SIZE);
	int bytes_received = recv(_clientFd, buffer.data(), BUFFER_SIZE, 0);
	if (bytes_received < 0)
	{
		err_msg("reading failed " + std::string(strerror(errno)));
		return -1;
	}
	if (bytes_received == 0)
	{
		debug_msg("Tab closed by client on FD" + std::to_string(_clientFd));
		return -1;
	}
	changeState(RECEIVING);
	std::vector<char> new_buffer(buffer.begin(), buffer.begin() + bytes_received);
	_request.updateBuffer(new_buffer);
	try
	{
		_request.parse();
	}
	catch(const Error& e)
	{
		err_msg("Request parsing failed (for client on FD" + std::to_string(_clientFd) + ") | reason: " + std::string(e.what()) + " | error code: " + std::to_string(e.code()));
		Response response;
		response.checkLocation();
		response.build_err(e.code(), "Request Parsing failed: " + std::string(e.what())); //really close connection??
		sendResponse(response.getResult());
		changeState(COMPLETE);
		return -1;
	}

    if (_request.is_complete())
	{
		changeState(SENDING);
		info_msg("Message recieved from client on FD " + std::to_string(_clientFd));
			// _request.debug_print();
        Response response(&_request);
		response.doMethod();
		response.build();
		// if (_request.get_path() != "/favicon.ico")
		// 	std::cout << "\nResponse:\n" << response.getResult() << std::endl;
		if (sendResponse(response.getResult()) < 0)
			return (-1);
		auto connection = _request.get_header("connection");
		if (!connection.empty())
		{
			if (connection[0] == "close")
				return -1;
		}
        _request.reset();
		changeState(COMPLETE);
    }
	return (0);
}

bool Client::hasTimedOut() const
{
	if (_state != COMPLETE)
	{
		auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - _lastActivityTime);
        if (duration.count() > _request_timeout)
            return true;
	}
	return (false);
}

int Client::sendResponse(std::string response)
{
	if(send(_clientFd, response.c_str(), response.length(), 0) > 0)
		info_msg("Response sent to client on FD " + std::to_string(_clientFd));
	else
	{
		err_msg("sending failed " + std::string(strerror(errno)));
		info_msg("closing connection to client (on FD" + std::to_string(_clientFd) + ")");
		return -1;
	}
	return (1);
}

void Client::changeState(Status newState)
{
	_state = newState;
	_lastActivityTime = std::chrono::steady_clock::now();
}

std::string Client::getSessionID(void)
{
	return (_sessionID);
}
