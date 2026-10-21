CC      = gcc
CFLAGS  = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Iinclude -g
LIB_SRC = src/entrada.c

.PHONY: all clean

all: bin/biblioteca

bin/biblioteca: src/main.c $(LIB_SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf bin
