NAME = pipex
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

LIBFT_DIR = libft
PRINTF_DIR = $(LIBFT_DIR)/printf

LIBFT = $(LIBFT_DIR)/libft.a
FTPRINTF = $(PRINTF_DIR)/libftprintf.a

SRC = main.c
OBJ = $(SRC:.c=.o)

all: $(LIBFT) $(FTPRINTF) $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(FTPRINTF):
	$(MAKE) -C $(PRINTF_DIR)

$(NAME): $(OBJ) $(LIBFT) $(FTPRINTF)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIBFT) $(FTPRINTF)

clean:
	rm -f $(OBJ)
	$(MAKE) -C $(LIBFT_DIR) clean
	$(MAKE) -C $(PRINTF_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean
	$(MAKE) -C $(PRINTF_DIR) fclean

re: fclean all

run: all
	$(MAKE) clean
	clear
	./pipex input.txt "cat" "ex.sh" output.txt
