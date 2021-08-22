CC = gcc
CFLAGS = -Wall -Wextra -g

build:
	$(CC) $(CFLAGS) src/* -o main
