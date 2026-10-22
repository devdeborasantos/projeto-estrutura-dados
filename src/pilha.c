#include "pilha.h"
#include <stdio.h>
#include <stdlib.h>

void pilha_init(Pilha *p) { p->topo = NULL; p->tamanho = 0; }

int pilha_push(Pilha *p, Acao a) {
    NoPilha *n = malloc(sizeof *n);
    if (!n) return -1;
    n->acao = a;
    n->prox = p->topo;
    p->topo = n;
    p->tamanho++;
    return 0;
}

int pilha_pop(Pilha *p, Acao *saida) {
    if (!p->topo) return -1;
    NoPilha *n = p->topo;
    if (saida) *saida = n->acao;
    p->topo = n->prox;
    free(n);
    p->tamanho--;
    return 0;
}

int pilha_peek(const Pilha *p, Acao *saida) {
    if (!p->topo) return -1;
    *saida = p->topo->acao;
    return 0;
}

int pilha_vazia(const Pilha *p) { return p->topo == NULL; }

int pilha_ultimas(const Pilha *p, int max, Acao *saida) {
    int n = 0;
    for (const NoPilha *x = p->topo; x && n < max; x = x->prox)
        saida[n++] = x->acao;
    return n;
}

const char *pilha_nome_acao(TipoAcao t) {
    switch (t) {
        case ACAO_CADASTRO:     return "CADASTRO";
        case ACAO_REMOCAO:      return "REMOCAO";
        case ACAO_ADD_CONEXAO:  return "ADD_CONEXAO";
        case ACAO_REM_CONEXAO:  return "REM_CONEXAO";
        default:                return "DESCONHECIDA";
    }
}

void pilha_imprimir_ultimas(const Pilha *p, int max) {
    if (pilha_vazia(p)) { printf("Historico vazio.\n"); return; }
    int i = 1;
    for (const NoPilha *x = p->topo; x && i <= max; x = x->prox, i++) {
        if (x->acao.id2 > 0)
            printf("%d. %-12s ids=%d,%d\n", i, pilha_nome_acao(x->acao.tipo), x->acao.id1, x->acao.id2);
        else
            printf("%d. %-12s id=%d\n", i, pilha_nome_acao(x->acao.tipo), x->acao.id1);
    }
}

void pilha_liberar(Pilha *p) {
    while (pilha_pop(p, NULL) == 0) { }
}
