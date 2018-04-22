CC=gcc
# GFLAGS=-ansi -Wall -pedantic-errors -g -lm -Werror
GFLAGS=-ggdb3 -Wall -Werror -pedantic-errors -lm -std=gnu11

all: clean lab06

lab06: lab06.c fila_prioridade.o
	$(CC) lab06.c fila_prioridade.o -o main $(GFLAGS)

fila_prioridade.o: fila_prioridade.h
	$(CC) fila_prioridade.c -c $(GFLAGS)

clean:
	rm -f *.o main