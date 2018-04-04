CC=gcc
# GFLAGS=-ansi -Wall -pedantic-errors -g -lm -Werror
GFLAGS=-ggdb3 -Wall -Werror -pedantic-errors -lm -std=gnu11

all: lab04

lab04: lab04.c fila.o
	$(CC) lab04.c fila.o -o lab04 $(GFLAGS)

fila.o: fila.c fila.h
	$(CC) fila.c -c $(GFLAGS)
clean:
	rm -f *.o
