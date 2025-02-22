/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 18:23:53 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/22 14:58:48 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Client.hpp"
#include "../../include/HTTP/requests/Request.hpp"
#include <unistd.h>

Client::Client(int clientFd, sockaddr_in addr, std::vector<ServerConfig> &conf, int port)
	: _clientFd(clientFd), _connected(true), _addr(addr), _request(conf, _sessionID), _request_timeout(2), _port(port)
{
	changeState(COMPLETE);
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
		response.build_err(e.code(), "Request Parsing failed: " + std::string(e.what()));
		_responseStr = response.getResult();
		changeState(SENDING);
		return (0);
	}

    if (_request.is_complete())
	{
		changeState(SENDING);
		info_msg("Message recieved from client on FD " + std::to_string(_clientFd));
		// if (_request.get_path() != "/favicon.ico")
		// 	_request.debug_print();
        Response response(&_request);
		response.doMethod();
		response.build();
		// if (_request.get_path() != "/favicon.ico")
		// 	std::cout << "\nResponse:\n" << response.getResult() << std::endl;
		_responseStr = response.getResult();
    }
	return (0);
}

bool Client::hasRequestTimedOut() const
{
	if (_state != COMPLETE)
	{
		auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - _lastActivityTime);
        if (duration.count() > _request_timeout)
		{
			debug_msg("Request timeout, closing ...");
            return true;
		}
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
	_state = COMPLETE;
	_request.reset();
	return (1);
}

void Client::changeState(Status newState)
{
	_state = newState;
	_lastActivityTime = std::chrono::steady_clock::now();
}

void Client::changeState(int newState)
{
	if(newState <= 0)
		_state = RECEIVING;
	else if(newState == 1)
		_state = SENDING;
	else
		_state = COMPLETE;
	_lastActivityTime = std::chrono::steady_clock::now();
}

std::string Client::getSessionID(void)
{
	return (_sessionID);
}

bool Client::isConnClosed(void)
{
	auto connection = _request.get_header("connection");
	if (!connection.empty())
	{
		if (connection[0] == "close")
			return true;
	}
	return false;
}

void Client::resetRequest(void)
{
	_request.reset();
}

bool Client::hasTimedOut(double timeoutTime) const
{
	if (_state == COMPLETE)
	{
		auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - _lastActivityTime);
        if (duration.count() > timeoutTime)
		{
			debug_msg("Hanging connection timeout, closing ...");
            return true;
		}
	}
	return (false);
}

void Client::setResponseStr(std::string str)
{
	_responseStr = str;
}

