#include "sistema.h"
#include "ordenacao.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sistema_init(Sistema *s) {
    bst_init(&s->bst);
    grafo_init(&s->grafo);
    pilha_init(&s->historico);
    s->simular_falha_vertice = 0;
    s->simular_falha_remocao_bst = 0;
}

void sistema_liberar(Sistema *s) {
    bst_liberar(&s->bst);
    pilha_liberar(&s->historico);
    grafo_init(&s->grafo);
}

const char *sistema_mensagem(int c) {
    switch (c) {
        case SIS_OK: return "Operacao concluida com sucesso.";
        case SIS_ID_INVALIDO: return "ID invalido (deve ser inteiro positivo).";
        case SIS_CAMPO_INVALIDO: return "Campo textual invalido (vazio ou acima de 79 caracteres).";
        case SIS_ANO_INVALIDO: return "Ano de publicacao fora do intervalo 1..2026.";
        case SIS_ID_DUPLICADO: return "Ja existe registro com este ID.";
        case SIS_CAPACIDADE: return "Capacidade maxima de 100 vertices atingida; cadastro rejeitado.";
        case SIS_ERRO_MEM: return "Falha de alocacao; operacao desfeita (rollback).";
        case SIS_NAO_ENCONTRADO: return "ID inexistente.";
        case SIS_LACO: return "Laco (A,A) nao permitido.";
        case SIS_CONEXAO_DUPLICADA: return "Conexao ja existe.";
        case SIS_CONEXAO_INEXISTENTE: return "Conexao inexistente.";
        case SIS_ERRO_CONSISTENCIA: return "Falha na remocao; estado restaurado (rollback).";
        default: return "Erro desconhecido.";
    }
}

static int map_gr(int r) {
    switch (r) {
        case GR_OK: return SIS_OK;
        case GR_ID_INVALIDO:
        case GR_VERTICE_INEXISTENTE: return SIS_NAO_ENCONTRADO;
        case GR_LACO: return SIS_LACO;
        case GR_ARESTA_DUPLICADA: return SIS_CONEXAO_DUPLICADA;
        case GR_ARESTA_INEXISTENTE: return SIS_CONEXAO_INEXISTENTE;
        case GR_CHEIO: return SIS_CAPACIDADE;
        case GR_SEM_MEMORIA: return SIS_ERRO_MEM;
        default: return SIS_ERRO_CONSISTENCIA;
    }
}

static int texto_valido(const char *t) {
    size_t n = strnlen(t, TAM_TEXTO);
    if (n == 0 || n > MAX_UTIL) return 0;
    for (size_t i = 0; i < n; i++)
        if (!isspace((unsigned char)t[i])) return 1;
    return 0;
}

static int registrar(Sistema *s, TipoAcao t, int a, int b) {
    Acao x = { t, a, b };
    return pilha_push(&s->historico, x);
}

/* Cadastro: BST -> vertice -> historico. Cada falha desfaz as etapas anteriores. */
int sistema_cadastrar(Sistema *s, const Livro *l) {
    if (l->id <= 0) return SIS_ID_INVALIDO;
    if (!texto_valido(l->titulo) || !texto_valido(l->autor)) return SIS_CAMPO_INVALIDO;
    if (l->ano < ANO_MIN || l->ano > ANO_MAX) return SIS_ANO_INVALIDO;

    int r = bst_inserir(&s->bst, l);
    if (r == BST_DUPLICADO) return SIS_ID_DUPLICADO;
    if (r != BST_OK) return SIS_ERRO_MEM;

    if (s->simular_falha_vertice) {                 /* simulacao de falha na criacao do vertice */
        s->simular_falha_vertice = 0;
        bst_remover(&s->bst, l->id);                /* rollback */
        return SIS_ERRO_MEM;
    }
    r = grafo_criar_vertice(&s->grafo, l->id);
    if (r != GR_OK) {
        bst_remover(&s->bst, l->id);                /* rollback da insercao na BST */
        return map_gr(r);
    }
    if (registrar(s, ACAO_CADASTRO, l->id, 0) != 0) {
        grafo_remover_vertice(&s->grafo, l->id);    /* rollback completo */
        bst_remover(&s->bst, l->id);
        return SIS_ERRO_MEM;
    }
    return SIS_OK;
}

/* Remocao: valida, guarda vizinhos, remove vertice, remove da BST; se falhar, restaura. */
int sistema_remover(Sistema *s, int id) {
    if (!bst_buscar(&s->bst, id)) return SIS_NAO_ENCONTRADO;

    int viz[GRAFO_MAX];
    int nv = grafo_vizinhos(&s->grafo, id, viz, GRAFO_MAX);
    if (nv < 0) return SIS_ERRO_CONSISTENCIA;       /* BST tem o registro mas o grafo nao */
    grafo_remover_vertice(&s->grafo, id);

    int falha = s->simular_falha_remocao_bst;
    s->simular_falha_remocao_bst = 0;
    if (falha || bst_remover(&s->bst, id) != BST_OK) {
        grafo_criar_vertice(&s->grafo, id);         /* rollback: recria vertice e arestas */
        for (int i = 0; i < nv; i++) grafo_add_aresta(&s->grafo, id, viz[i]);
        return SIS_ERRO_CONSISTENCIA;
    }
    if (registrar(s, ACAO_REMOCAO, id, 0) != 0)
        fprintf(stderr, "Aviso: nao foi possivel registrar no historico.\n");
    return SIS_OK;
}

int sistema_add_conexao(Sistema *s, int a, int b) {
    int r = grafo_add_aresta(&s->grafo, a, b);
    if (r != GR_OK) return map_gr(r);
    if (registrar(s, ACAO_ADD_CONEXAO, a, b) != 0)
        fprintf(stderr, "Aviso: nao foi possivel registrar no historico.\n");
    return SIS_OK;
}

int sistema_rem_conexao(Sistema *s, int a, int b) {
    int r = grafo_remover_aresta(&s->grafo, a, b);
    if (r != GR_OK) return map_gr(r);
    if (registrar(s, ACAO_REM_CONEXAO, a, b) != 0)
        fprintf(stderr, "Aviso: nao foi possivel registrar no historico.\n");
    return SIS_OK;
}

int sistema_conexao_direta(const Sistema *s, int a, int b, int *resultado) {
    return map_gr(grafo_conectados(&s->grafo, a, b, resultado));
}

int sistema_alcancaveis(const Sistema *s, int origem, Lista *saida) {
    return map_gr(grafo_bfs(&s->grafo, origem, saida));
}

int sistema_caminho(const Sistema *s, int a, int b, int *resultado) {
    return map_gr(grafo_existe_caminho(&s->grafo, a, b, resultado));
}

int sistema_ordenar(const Sistema *s, Livro **vetor, int *n) {
    *vetor = NULL;
    *n = s->bst.tamanho;
    if (*n == 0) return SIS_OK;
    Livro *v = malloc((size_t)*n * sizeof(Livro));
    if (!v) return SIS_ERRO_MEM;
    bst_copiar(&s->bst, v, *n);
    ordenar_livros(v, *n);
    *vetor = v;
    return SIS_OK;
}
