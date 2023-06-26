CC=gcc
CFLAGS=-Werror -Wall -Wextra -std=c11 -O2 -pedantic -D_FORTIFY_SOURCE=2
NAME=s21_grep

all: clean s21_grep

build: s21_grep

s21_grep:
	$(CC) $(CFLAGS) $(NAME).c -o $(NAME)

clean:
	rm -rf *.o $(NAME) *.log

rebuild: clean all

style:
	clang-format -n --style=Google *.[ch]
	cppcheck *.[ch]
