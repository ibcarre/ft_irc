NAME = ircserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -g3 -MMD -std=c++98

SRCS = main.cpp\
	Command.cpp\
	User.cpp\
	Server.cpp\

OBJ_DIR = obj

OBJ = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

DEPENDS := $(OBJ:.o=.d)


all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPENDS)
