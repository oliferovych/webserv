/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketPoll.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 15:24:49 by dolifero          #+#    #+#             */
/*   Updated: 2025/01/12 16:10:00 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <poll.h>
#include <sys/socket.h>

class SocketPoll
{
	private:
		std::vector<struct pollfd> _fds;
		int _timeout;
	public:
		SocketPoll();
		~SocketPoll();
		//methods
		void addFd(int fd);
		void removeFd(int fd);
		//checkers
		int wait();
		bool canRead(int index);
		//getters
		int getFd(int index);
		int get_pending_fd();
		size_t size() const;
		std::vector<struct pollfd> getFds();
};