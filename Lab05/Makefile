CC=gcc
GFLAGS=-ansi -Wall -pedantic-errors -g -lm -Werror -pg
# GFLAGS=-ggdb3 -Wall -Werror -pedantic-errors -lm -std=gnu11 -O2 -pg

all: main

main: lab05.o pilha.o
	$(CC) lab05.o pilha.o -o main $(GFLAGS)

lab05.o: lab05.c
	$(CC) lab05.c -c $(GFLAGS)

pilha.o: pilha.c pilha.h
	$(CC) pilha.c -c $(GFLAGS)

clean:
	rm -f main *.o
