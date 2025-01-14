/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/13 18:19:28 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/13 18:43:44 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../global.hpp"

class Client
{
	private:
		int _clientFd;
		bool _isFdOpen;
		sockaddr_in _addr;
	public:
		Client(int clientFd, sockaddr_in addr);
		~Client();
		const int &getClientFd() const { return _clientFd; }
		const sockaddr_in &getAddr() const { return _addr; }
};
