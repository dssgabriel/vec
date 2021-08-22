CC = gcc
CFLAGS = -Wall -Wextra -g

test: 
	$(CC) $(CFLAGS) src/vec.c test/test.c -o main
	./main
