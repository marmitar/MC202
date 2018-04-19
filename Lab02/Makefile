CC=gcc
GFLAGS=-g -ansi -Wall -pedantic-errors -Werror -lm
# GFLAGS=-Wall -pedantic-errors -Werror -lm -std=c11 -ggdb3

all: compile

compile: vetor.o lab02.o
	$(CC) vetor.o lab02.o -o lab02 $(GFLAGS)

vetor.o: vetor.c vetor.h
	$(CC) vetor.c -c $(GFLAGS)

lab02.o: lab02.c
	$(CC) lab02.c -c $(GFLAGS)

clean:
	rm -f *.o lab02
