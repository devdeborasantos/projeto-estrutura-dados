#ifndef FILA_H
#define FILA_H

/* Fila dinamica encadeada de inteiros (usada na BFS). */
typedef struct NoFila {
    int valor;
    struct NoFila *prox;
} NoFila;

typedef struct {
    NoFila *ini, *fim;
    int tamanho;
} Fila;

void fila_init(Fila *f);
int  fila_enfileirar(Fila *f, int valor);     /* 0 ok, -1 sem memoria */
int  fila_desenfileirar(Fila *f, int *valor); /* 0 ok, -1 vazia */
int  fila_vazia(const Fila *f);
void fila_liberar(Fila *f);

#endif
