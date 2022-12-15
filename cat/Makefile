CC=gcc
CFLAGS=-Werror -Wall -Wextra -std=c11 -O2 -pedantic -D_FORTIFY_SOURCE=2
NAME=s21_cat

all: clean s21_cat

s21_cat:
	$(CC) $(CFLAGS) $(NAME).c -o $(NAME)

clean:
	rm -rf *.o $(NAME) *.log

rebuild: clean all
