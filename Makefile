re: fclean all

all: my_tar.c
	@gcc my_tar.c -g -fsanitize=address -o my_tar

fclean:
	@rm -f my_tar
	@rm -f a.txt

.PHONY: fclean

flags:
	-Wall -Wextra -Werror 