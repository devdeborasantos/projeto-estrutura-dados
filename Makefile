CC      = gcc
CFLAGS  = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Iinclude -g
LIB_SRC = src/entrada.c src/bst.c src/grafo.c src/lista.c src/pilha.c src/fila.c src/ordenacao.c src/sistema.c

.PHONY: all test clean

all: bin/biblioteca

bin/biblioteca: src/main.c $(LIB_SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

bin/testes: tests/test_main.c $(LIB_SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

test: bin/testes
	./bin/testes

clean:
	rm -rf bin
