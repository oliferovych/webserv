/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dolifero <dolifero@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/11 21:35:37 by dolifero          #+#    #+#             */
/*   Updated: 2025/02/21 16:31:52 by dolifero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// Internal includes
#include "server/Socket.hpp"
#include "server/SocketPoll.hpp"
#include "server/Server.hpp"
#include "HTTP/requests/Request.hpp"
#include "HTTP/response/Response.hpp"
#include "server/Client.hpp"
#include "utils/utils.hpp"

std::string cgi_handler(const std::string &path);

#include <sstream>

// Colors
#define FG_BLACK   		"\033[30m"
#define FG_RED     		"\033[31m"
#define FG_GREEN   		"\033[32m"
#define FG_BOLDGREEN	"\033[1;32m"
#define FG_YELLOW  		"\033[33m"
#define FG_BLUE    		"\033[34m"
#define FG_BOLDBLUE		"\033[1;34m"
#define FG_MAGENTA 		"\033[35m"
#define FG_CYAN    		"\033[36m"
#define FG_WHITE   		"\033[37m"

#define RESET      		"\033[0m"
#define BOLD       		"\033[1m"
#define DIM        		"\033[2m"
#define ITALIC     		"\033[3m"
#define UNDERLINE  		"\033[4m"
#define BLINK      		"\033[5m"
#define REVERSE    		"\033[7m"
#define HIDDEN     		"\033[8m"
