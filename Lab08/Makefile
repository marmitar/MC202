CC=gcc
GFLAGS=-ansi -Wall -Werror -pedantic-errors -g -lm
# GFLAGS=-ggdb3 -Wall -Werror -pedantic-errors -g -lm -std=gnu11

all: main

main: abb.o lab08.c
	$(CC) lab08.c abb.o -o main $(GFLAGS)

abb.o: abb.c abb.h
	$(CC) abb.c -c $(GFLAGS)

clean:
	rm -f *.o

