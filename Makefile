CC      = gcc
CFLAGS  = -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Iinclude -g
LIB_SRC = src/entrada.c src/lista.c src/fila.c src/pilha.c src/ordenacao.c src/bst.c

.PHONY: all test asan valgrind clean

all: bin/biblioteca

bin/biblioteca: src/main.c $(LIB_SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

bin/testes: tests/test_main.c $(LIB_SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $^

test: bin/testes
	./bin/testes

asan:
	mkdir -p bin
	$(CC) $(CFLAGS) -fsanitize=address,undefined -o bin/testes_asan tests/test_main.c $(LIB_SRC)
	./bin/testes_asan

valgrind: bin/testes
	valgrind --leak-check=full --error-exitcode=1 ./bin/testes

clean:
	rm -rf bin