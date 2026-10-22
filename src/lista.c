#include "lista.h"
#include <stdio.h>
#include <stdlib.h>

void lista_init(Lista *l) { l->ini = l->fim = NULL; l->tamanho = 0; }

int lista_inserir_fim(Lista *l, int valor) {
    NoLista *n = malloc(sizeof *n);
    if (!n) return -1;
    n->valor = valor;
    n->prox = NULL;
    if (l->fim) l->fim->prox = n; else l->ini = n;
    l->fim = n;
    l->tamanho++;
    return 0;
}

int lista_contem(const Lista *l, int valor) {
    for (const NoLista *p = l->ini; p; p = p->prox)
        if (p->valor == valor) return 1;
    return 0;
}

void lista_imprimir(const Lista *l) {
    for (const NoLista *p = l->ini; p; p = p->prox)
        printf("%d%s", p->valor, p->prox ? " -> " : "");
    printf("\n");
}

void lista_liberar(Lista *l) {
    NoLista *p = l->ini;
    while (p) { NoLista *t = p->prox; free(p); p = t; }
    lista_init(l);
}
