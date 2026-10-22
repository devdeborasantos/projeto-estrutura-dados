#include "bst.h"
#include <stdlib.h>

void bst_init(Bst *b) { b->raiz = NULL; b->tamanho = 0; }

int bst_inserir(Bst *b, const Livro *l) {
    NoBst **p = &b->raiz;
    while (*p) {
        if (l->id == (*p)->dado.id) return BST_DUPLICADO;
        p = (l->id < (*p)->dado.id) ? &(*p)->esq : &(*p)->dir;
    }
    NoBst *n = malloc(sizeof *n);
    if (!n) return BST_SEM_MEMORIA;
    n->dado = *l;
    n->esq = n->dir = NULL;
    *p = n;
    b->tamanho++;
    return BST_OK;
}

const Livro *bst_buscar(const Bst *b, int id) {
    const NoBst *p = b->raiz;
    while (p) {
        if (id == p->dado.id) return &p->dado;
        p = (id < p->dado.id) ? p->esq : p->dir;
    }
    return NULL;
}

int bst_remover(Bst *b, int id) {
    NoBst **p = &b->raiz;
    while (*p && (*p)->dado.id != id)
        p = (id < (*p)->dado.id) ? &(*p)->esq : &(*p)->dir;
    if (!*p) return BST_NAO_ENCONTRADO;

    NoBst *alvo = *p;
    if (!alvo->esq) {                 /* 0 filhos ou so filho direito */
        *p = alvo->dir;
    } else if (!alvo->dir) {          /* so filho esquerdo */
        *p = alvo->esq;
    } else {                          /* 2 filhos: substitui pelo sucessor in-ordem */
        NoBst **ps = &alvo->dir;
        while ((*ps)->esq) ps = &(*ps)->esq;
        NoBst *suc = *ps;
        *ps = suc->dir;               /* destaca o sucessor */
        suc->esq = alvo->esq;
        suc->dir = alvo->dir;
        *p = suc;
    }
    free(alvo);
    b->tamanho--;
    return BST_OK;
}

static void em_ordem(const NoBst *n, BstVisita v, void *ctx) {
    if (!n) return;
    em_ordem(n->esq, v, ctx);
    v(&n->dado, ctx);
    em_ordem(n->dir, v, ctx);
}

void bst_em_ordem(const Bst *b, BstVisita visita, void *ctx) { em_ordem(b->raiz, visita, ctx); }

const Livro *bst_menor(const Bst *b) {
    const NoBst *p = b->raiz;
    if (!p) return NULL;
    while (p->esq) p = p->esq;
    return &p->dado;
}

const Livro *bst_maior(const Bst *b) {
    const NoBst *p = b->raiz;
    if (!p) return NULL;
    while (p->dir) p = p->dir;
    return &p->dado;
}

static int altura(const NoBst *n) {
    if (!n) return 0;
    int e = altura(n->esq), d = altura(n->dir);
    return 1 + (e > d ? e : d);
}
int bst_altura(const Bst *b) { return altura(b->raiz); }

typedef struct { Livro *vet; int cap; int n; } CtxCopia;
static void copiar_cb(const Livro *l, void *c) {
    CtxCopia *x = c;
    if (x->n < x->cap) x->vet[x->n++] = *l;
}
int bst_copiar(const Bst *b, Livro *vet, int cap) {
    CtxCopia c = { vet, cap, 0 };
    bst_em_ordem(b, copiar_cb, &c);
    return c.n;
}

static void liberar(NoBst *n) {
    if (!n) return;
    liberar(n->esq);
    liberar(n->dir);
    free(n);
}
void bst_liberar(Bst *b) { liberar(b->raiz); bst_init(b); }
