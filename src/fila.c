#include "fila.h"
#include <stdlib.h>

void fila_init(Fila *f) { f->ini = f->fim = NULL; f->tamanho = 0; }

int fila_enfileirar(Fila *f, int valor) {
    NoFila *n = malloc(sizeof *n);
    if (!n) return -1;
    n->valor = valor;
    n->prox = NULL;
    if (f->fim) f->fim->prox = n; else f->ini = n;
    f->fim = n;
    f->tamanho++;
    return 0;
}

int fila_desenfileirar(Fila *f, int *valor) {
    if (!f->ini) return -1;
    NoFila *n = f->ini;
    *valor = n->valor;
    f->ini = n->prox;
    if (!f->ini) f->fim = NULL;
    free(n);
    f->tamanho--;
    return 0;
}

int fila_vazia(const Fila *f) { return f->ini == NULL; }

void fila_liberar(Fila *f) {
    int v;
    while (fila_desenfileirar(f, &v) == 0) { }
}
