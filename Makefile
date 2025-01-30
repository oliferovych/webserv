# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tomecker <tomecker@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/09 21:08:00 by dolifero          #+#    #+#              #
#    Updated: 2025/01/30 16:07:56 by tomecker         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			=	webserv

CC				=	c++

CFLAGS			=	-Wall -Werror -Wextra -std=c++17

SRC_FILES		=	$(shell find $(SRC_DIR) -name '*.cpp')

OBJ_DIR			=	./obj
SRC_DIR 		= 	./src

UPLOADS			=	./defconfigs/test1/uploads

OBJ_FILES		= 	$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

all:	$(NAME)

$(NAME):	$(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "clean $(NAME)"
	@rm -rf $(OBJ_DIR)
	@rm -rf $(UPLOADS)

fclean:	clean
	@echo "fclean $(NAME)"
	@rm -f $(NAME)

re:	fclean all

.PHONY:		all clean fclean re
