/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 18:23:53 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/21 22:27:46 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Client.hpp"
#include <unistd.h>

Client::Client(int clientFd, sockaddr_in addr)
	: _clientFd(clientFd), _connected(true), _addr(addr), _state(COMPLETE)
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
	std::vector<char> new_buffer(buffer.begin(), buffer.begin() + bytes_received);
	_state = RECEIVING;
	_request.updateBuffer(new_buffer);
	_lastActivityTime = std::chrono::steady_clock::now();
	try
	{
		_request.parse();
	}
	catch(const Error& e)
	{
		err_msg("Parsing failed (for client on FD" + std::to_string(_clientFd) + ") | reason: " + std::string(e.what()) + " | error code: " + std::to_string(e.code()));
		Response response(_request);
		response.setCode(e.code());
		response.build_err();
		sendResponse(response.getResult());
		return -1;
	}

	// char response[] = 	"HTTP/1.1 200 OK\r\n"
	// 					"Content-Type: text/html\r\n"
	// 					"Content-Length: 416\r\n"
	// 					"\r\n"
	// 					"<!DOCTYPE html>\r\n"
	// 					"<html><head><style>\r\n"
	// 					"body {display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0;}\r\n"
	// 					"a {font-size: 48px; text-decoration: none; transition: all 0.3s; text-decoration-line: underline;}\r\n"
	// 					"a:hover {font-size:52px; text-decoration-line: underline;}\r\n"
	// 					"</style></head>\r\n"
	// 					"<body><a href='https://www.youtube.com/watch?v=xvFZjo5PgG0'>Something cool, click on me!</a></body></html>";

    if (_request.is_complete())
	{
		_state = SENDING;
		_lastActivityTime = std::chrono::steady_clock::now();
		info_msg("Message recieved from client on FD " + std::to_string(_clientFd));
		_request.debug_print();
        Response response(_request);
		response.doMethod();
		response.build();
		std::cout << "\nResponse:\n" << response.getResult() << std::endl;
		if (sendResponse(response.getResult()) < 0)
			return (-1);
		auto connection = _request.get_header("connection");
		if (!connection.empty())
		{
			if (connection[0] == "close")
				return -1;
		}
        _request.reset();
		_state = COMPLETE;
    }
	return (0);
}

bool Client::hasTimedOut() const
{
	if (_state != COMPLETE)
	{
		auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - _lastActivityTime);
        if (duration.count() > TIMER)
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
