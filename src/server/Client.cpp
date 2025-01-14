/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 18:23:53 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/13 18:27:15 by dolifero         ###   ########.fr       */
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
