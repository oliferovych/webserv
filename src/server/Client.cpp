/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 18:23:53 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/14 20:21:06 by tomecker         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Client.hpp"
#include <unistd.h>

Client::Client(int clientFd, sockaddr_in addr)
	: _clientFd(clientFd), _isFdOpen(true), _addr(addr)
{
}

Client::~Client()
{
	if (_isFdOpen)
		close(_clientFd);
}

int Client::handle_message()
{
	std::vector<char> buffer(BUFFER_SIZE);
	int bytes_received = recv(_clientFd, buffer.data(), BUFFER_SIZE, 0);
	if (bytes_received < 0)
	{
		err_msg("reading failed " + std::string(strerror(errno)));
		info_msg("closing connection to client (on FD" + std::to_string(_clientFd) + ")");
			return -1;
	}

	if (bytes_received == 0)
	{
		info_msg("client disconnected (on FD" + std::to_string(_clientFd) + ")");
		return (-1);
	}
	std::vector<char> new_buffer(buffer.begin(), buffer.begin() + bytes_received);
	_request.updateBuffer(new_buffer);
	try
	{
		_request.parse();
	}
	catch(const HTTPException& e)
	{
		err_msg("Parsing failed (for client on FD" + std::to_string(_clientFd) + ") | reason:" + std::string(e.what()) + " | error code: " + std::to_string(e.code()));
		// response(req, code());
		info_msg("closing connection to client (on FD" + std::to_string(_clientFd) + ")");
		return -1;
	}
	
	char response[] = 	"HTTP/1.1 200 OK\r\n"
						"Content-Type: text/html\r\n"
						"Content-Length: 416\r\n"
						"\r\n"
						"<!DOCTYPE html>\r\n"
						"<html><head><style>\r\n"
						"body {display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0;}\r\n"
						"a {font-size: 48px; text-decoration: none; transition: all 0.3s; text-decoration-line: underline;}\r\n"
						"a:hover {font-size:52px; text-decoration-line: underline;}\r\n"
						"</style></head>\r\n"
						"<body><a href='https://www.youtube.com/watch?v=xvFZjo5PgG0'>Something cool, click on me!</a></body></html>";
								
    if (_request.is_complete())
	{
		info_msg("Message recieved from client on FD " + std::to_string(_clientFd));
		_request.debug_print();
        // response(req);
			if(send(_clientFd, response, strlen(response), 0) > 0)
				info_msg("Response sent to client on FD " + std::to_string(_clientFd));
							// + " from " + std::string(inet_ntoa(_addr.sin_addr)));
			else
			{
				err_msg("reading failed " + std::string(strerror(errno)));
				info_msg("closing connection to client (on FD" + std::to_string(_clientFd) + ")");
				return -1;
			}
        _request.reset();
    }
	return (0);
}

