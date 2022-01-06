CC = gcc
SRC = $(wildcard src/test.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c) $(wildcard src/*/*/*/*.c)
OBJ = $(SRC:.c=.o)
BIN = test

%.o: %.c
	$(CC) -Wall -Wextra -pedantic -Werror -O3 -c $< -o $@

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) -lm

.PHONY: clean
clean:
	@rm $(OBJ)
	@rm $(BIN)
