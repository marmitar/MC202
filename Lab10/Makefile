TARGET = main
CC = gcc
# CFLAGS = -ansi -Wall -pedantic-errors -Werror -g
GFLAGS = -ggdb3 -Wall -Werror -pedantic-errors -g -std=gnu11
LIBS = -lm

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

%.o: %.c %.h
	$(CC) $(CFLAGS) $(LIBS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o

