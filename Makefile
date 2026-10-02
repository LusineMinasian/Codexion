NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -I includes
SRCS = srcs/main.c srcs/parsing.c srcs/heap.c
HEAD = includes/codexion.h
OBJS = $(SRCS:.c=.o)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEAD)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re