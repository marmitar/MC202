CC=gcc
GFLAGS=-ansi -Wall -pedantic-errors -g -lm

all: main

main: aluno.o professor.o lab01.o main.c
	$(CC) main.c aluno.o professor.o lab01.o -o main $(GFLAGS)

aluno.o: aluno.c aluno.h
	$(CC) aluno.c -c $(GFLAGS)

professor.o: professor.c professor.h
	$(CC) professor.c -c $(GFLAGS)

lab01.o: lab01.c lab01.h
	$(CC) lab01.c -c $(GFLAGS)

clean:
	rm -f *.o