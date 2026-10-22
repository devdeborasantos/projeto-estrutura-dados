#ifndef BST_H
#define BST_H

#include "registro.h"

typedef struct NoBst {
    Livro dado;
    struct NoBst *esq, *dir;
} NoBst;

typedef struct {
    NoBst *raiz;
    int tamanho;
} Bst;

enum { BST_OK = 0, BST_DUPLICADO = -1, BST_SEM_MEMORIA = -2, BST_NAO_ENCONTRADO = -3 };

typedef void (*BstVisita)(const Livro *l, void *ctx);

void         bst_init(Bst *b);
int          bst_inserir(Bst *b, const Livro *l);
const Livro *bst_buscar(const Bst *b, int id);
int          bst_remover(Bst *b, int id);          /* 0, 1 ou 2 filhos */
void         bst_em_ordem(const Bst *b, BstVisita visita, void *ctx);
const Livro *bst_menor(const Bst *b);              /* NULL se vazia */
const Livro *bst_maior(const Bst *b);
int          bst_altura(const Bst *b);
/* Copia os registros (valores independentes) em ordem de ID; retorna quantidade. */
int          bst_copiar(const Bst *b, Livro *vet, int cap);
void         bst_liberar(Bst *b);

#endif
