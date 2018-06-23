TARGET = main
CC = gcc
# CFLAGS = -ansi -Wall -pedantic-errors -Werror -g
CFLAGS = -std=gnu11 -Wall -pedantic-errors -Werror -g
DEBUG = # -DDEBUG
LIBS = -lm

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS) $(DEBUG)

%.o: %.c %.h
	$(CC) $(CFLAGS) $(LIBS) $(DEBUG) -c $< -o $@

clean:
	rm -f $(TARGET) *.o
