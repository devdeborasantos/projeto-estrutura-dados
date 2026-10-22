#ifndef LISTA_H
#define LISTA_H

/* Lista encadeada simples de inteiros, com ponteiro para o fim (insercao O(1) no fim). */
typedef struct NoLista {
    int valor;
    struct NoLista *prox;
} NoLista;

typedef struct {
    NoLista *ini, *fim;
    int tamanho;
} Lista;

void lista_init(Lista *l);
int  lista_inserir_fim(Lista *l, int valor);   /* 0 ok, -1 sem memoria */
int  lista_contem(const Lista *l, int valor);
void lista_imprimir(const Lista *l);
void lista_liberar(Lista *l);

#endif
