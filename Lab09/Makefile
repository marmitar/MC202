SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
CC=gcc
GFLAGS=-ansi -Wall -pedantic-errors -Werror -g -lm
# GFLAGS=-ggdb3 -Wall -Werror -pedantic-errors -g -lm -std=gnu11

all: main

main: $(OBJ)
	$(CC) $(GFLAGS) $(OBJ) -o $@

%.o: %.c
	$(CC) $(GFLAGS) -c $< -o $@

clean:
	rm -f *.o main
