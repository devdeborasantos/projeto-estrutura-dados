#ifndef SISTEMA_H
#define SISTEMA_H

#include "bst.h"
#include "grafo.h"
#include "pilha.h"
#include "lista.h"
#include "registro.h"

/* Camada de integracao BST + grafo + historico, com rollback. */
typedef enum {
    SIS_OK = 0,
    SIS_ID_INVALIDO = -1,
    SIS_CAMPO_INVALIDO = -2,
    SIS_ANO_INVALIDO = -3,
    SIS_ID_DUPLICADO = -4,
    SIS_CAPACIDADE = -5,
    SIS_ERRO_MEM = -6,
    SIS_NAO_ENCONTRADO = -7,
    SIS_LACO = -8,
    SIS_CONEXAO_DUPLICADA = -9,
    SIS_CONEXAO_INEXISTENTE = -10,
    SIS_ERRO_CONSISTENCIA = -11
} StatusSis;

typedef struct {
    Bst    bst;
    Grafo  grafo;
    Pilha  historico;
    /* Ganchos de simulacao de falha (uso exclusivo em testes; disparam uma vez). */
    int    simular_falha_vertice;
    int    simular_falha_remocao_bst;
} Sistema;

void        sistema_init(Sistema *s);
void        sistema_liberar(Sistema *s);
const char *sistema_mensagem(int codigo);

int sistema_cadastrar(Sistema *s, const Livro *l);
int sistema_remover(Sistema *s, int id);
int sistema_add_conexao(Sistema *s, int a, int b);
int sistema_rem_conexao(Sistema *s, int a, int b);
int sistema_conexao_direta(const Sistema *s, int a, int b, int *resultado);
int sistema_alcancaveis(const Sistema *s, int origem, Lista *saida);
int sistema_caminho(const Sistema *s, int a, int b, int *resultado);
/* Aloca vetor de copias ordenado por titulo. O chamador libera com free(). */
int sistema_ordenar(const Sistema *s, Livro **vetor, int *n);

#endif
