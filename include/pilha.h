#ifndef PILHA_H
#define PILHA_H

typedef enum {
    ACAO_CADASTRO = 1,
    ACAO_REMOCAO,
    ACAO_ADD_CONEXAO,
    ACAO_REM_CONEXAO
} TipoAcao;

typedef struct {
    TipoAcao tipo;
    int id1;
    int id2;   /* 0 quando a acao envolve um unico ID */
} Acao;

/* Pilha dinamica encadeada: limitada apenas pela memoria. */
typedef struct NoPilha {
    Acao acao;
    struct NoPilha *prox;
} NoPilha;

typedef struct {
    NoPilha *topo;
    int tamanho;
} Pilha;

void        pilha_init(Pilha *p);
int         pilha_push(Pilha *p, Acao a);          /* 0 ok, -1 sem memoria */
int         pilha_pop(Pilha *p, Acao *saida);      /* 0 ok, -1 vazia */
int         pilha_peek(const Pilha *p, Acao *saida);/* 0 ok, -1 vazia */
int         pilha_vazia(const Pilha *p);
/* Consulta NAO destrutiva: copia ate 'max' acoes (mais recente primeiro). Retorna quantidade. */
int         pilha_ultimas(const Pilha *p, int max, Acao *saida);
void        pilha_imprimir_ultimas(const Pilha *p, int max);
const char *pilha_nome_acao(TipoAcao t);
void        pilha_liberar(Pilha *p);

#endif
