# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/09 21:08:00 by dolifero          #+#    #+#              #
#    Updated: 2025/01/13 18:58:44 by tomecker         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			=	webserv

CC				=	c++

CFLAGS			=	-Wall -Werror -Wextra -std=c++17

SRC_FILES		=	$(shell find $(SRC_DIR) -name '*.cpp')

OBJ_DIR			=	./obj
SRC_DIR 		= 	./src

OBJ_FILES		= 	$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

all:	$(NAME)

$(NAME):	$(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)

fclean:	clean
	@rm -f $(NAME)

re:	fclean all

.PHONY:		all clean fclean re