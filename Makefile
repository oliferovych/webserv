# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tecker <tecker@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/09 21:08:00 by dolifero          #+#    #+#              #
#    Updated: 2025/01/13 15:00:23 by tecker           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv
CC			= c++
CFLAGS		= -Wall -Wextra -Werror -std=c++17
RM			= rm -f

SRC_FILES	= $(shell find src -name '*.cpp')
OBJ_FILES	= $(SRC_FILES:.cpp=.o)

all:		$(NAME)

$(NAME):	$(OBJ_FILES)
			$(CC) $(CFLAGS) -o $(NAME) $(OBJ_FILES)

%.o:		%.cpp
			$(CC) $(CFLAGS) -c $< -o $@

clean:
			$(RM) $(OBJ_FILES)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re 

