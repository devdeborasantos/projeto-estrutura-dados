#ifndef GRAFO_H
#define GRAFO_H

#include "lista.h"

#define GRAFO_MAX 100

/* Grafo nao direcionado e nao ponderado, matriz de adjacencia com mapeamento
 * ID -> indice compacto (id[i] guarda o ID do vertice no slot i). */
typedef struct {
    int  id[GRAFO_MAX];
    char ativo[GRAFO_MAX];
    char adj[GRAFO_MAX][GRAFO_MAX];
    int  n;
} Grafo;

enum {
    GR_OK = 0,
    GR_VERTICE_DUPLICADO = -1,
    GR_CHEIO = -2,
    GR_VERTICE_INEXISTENTE = -3,
    GR_LACO = -4,
    GR_ARESTA_DUPLICADA = -5,
    GR_ARESTA_INEXISTENTE = -6,
    GR_SEM_MEMORIA = -7,
    GR_ID_INVALIDO = -8
};

void grafo_init(Grafo *g);
int  grafo_criar_vertice(Grafo *g, int id);
int  grafo_remover_vertice(Grafo *g, int id);       /* remove tambem as arestas incidentes */
int  grafo_existe(const Grafo *g, int id);
int  grafo_num_vertices(const Grafo *g);
int  grafo_add_aresta(Grafo *g, int a, int b);
int  grafo_remover_aresta(Grafo *g, int a, int b);
int  grafo_conectados(const Grafo *g, int a, int b, int *resultado);   /* conexao direta */
/* Vizinhos de id em ordem crescente de ID. Retorna quantidade (>=0) ou codigo negativo. */
int  grafo_vizinhos(const Grafo *g, int id, int *saida, int cap);
/* BFS: insere em 'saida' (lista vazia e inicializada) os alcancaveis, incluindo a origem. */
int  grafo_bfs(const Grafo *g, int origem, Lista *saida);
int  grafo_existe_caminho(const Grafo *g, int a, int b, int *resultado);

#endif
