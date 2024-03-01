##
## EPITECH PROJECT, 2024
## Makefile
## File description:
## Makefile
##

NAME = EXECUTABLE_NAME

MAIN = src/main.c

SRCS =

TESTS_SRC = tests/test_main.c \

CFLAGS = -Ofast -march=native -flto=auto -Wall -Wextra -std=gnu2x -I./include

CDEBUGFLAGS = -Og -g3 -std=gnu2x -I./include


all:
	@gcc -o $(NAME) $(MAIN) $(SRCS) $(CFLAGS)

debug:
	@gcc -o $(NAME) $(MAIN) $(SRCS) $(CDEBUGFLAGS)

clean:
	@rm -rf *.o .idea/
	@find . -type f,d \( -name "*~" -o -name "\#*\#" \) -delete
	@find . -type f,d -name "vgcore*" -delete
	@find . -type f,d -name "*.gcno" -delete
	@find . -type f,d -name "*.gcda" -delete

fclean: clean
	@find . -name $(NAME) -delete
	@find . -name test_bin -delete

re:	fclean all

tests_run: fclean
	@gcc -o test_bin $(TESTS_SRC) $(SRCS) --coverage -lcriterion $(CDEBUGFLAGS)
	@./test_bin

.PHONY: all clean fclean re tests_run debug
