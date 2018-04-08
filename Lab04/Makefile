CC=gcc
# GFLAGS=-ansi -Wall -pedantic-errors -g -lm -Werror
GFLAGS=-ggdb3 -Wall -Werror -pedantic-errors -lm -std=gnu11

all: lab04

lab04: lab04.c fila.o participante.o
	$(CC) lab04.c fila.o participante.o -o lab04 $(GFLAGS)

fila.o: fila.c fila.h
	$(CC) fila.c -c $(GFLAGS)

participante.o: participante.c participante.h
	$(CC) participante.c -c $(GFLAGS)

clean:
	rm -f *.o
