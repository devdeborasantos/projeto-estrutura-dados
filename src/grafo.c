#include "grafo.h"
#include "fila.h"
#include <string.h>

void grafo_init(Grafo *g) { memset(g, 0, sizeof *g); }

static int indice(const Grafo *g, int id) {
    if (id <= 0) return -1;
    for (int i = 0; i < GRAFO_MAX; i++)
        if (g->ativo[i] && g->id[i] == id) return i;
    return -1;
}

int grafo_existe(const Grafo *g, int id) { return indice(g, id) >= 0; }
int grafo_num_vertices(const Grafo *g) { return g->n; }

int grafo_criar_vertice(Grafo *g, int id) {
    if (id <= 0) return GR_ID_INVALIDO;
    if (indice(g, id) >= 0) return GR_VERTICE_DUPLICADO;
    if (g->n >= GRAFO_MAX) return GR_CHEIO;
    for (int i = 0; i < GRAFO_MAX; i++) {
        if (!g->ativo[i]) {
            g->ativo[i] = 1;
            g->id[i] = id;
            for (int j = 0; j < GRAFO_MAX; j++) { g->adj[i][j] = 0; g->adj[j][i] = 0; }
            g->n++;
            return GR_OK;
        }
    }
    return GR_CHEIO;
}

int grafo_remover_vertice(Grafo *g, int id) {
    int i = indice(g, id);
    if (i < 0) return GR_VERTICE_INEXISTENTE;
    for (int j = 0; j < GRAFO_MAX; j++) { g->adj[i][j] = 0; g->adj[j][i] = 0; }
    g->ativo[i] = 0;
    g->id[i] = 0;
    g->n--;
    return GR_OK;
}

int grafo_add_aresta(Grafo *g, int a, int b) {
    int i = indice(g, a), j = indice(g, b);
    if (i < 0 || j < 0) return GR_VERTICE_INEXISTENTE;
    if (i == j) return GR_LACO;
    if (g->adj[i][j]) return GR_ARESTA_DUPLICADA;
    g->adj[i][j] = g->adj[j][i] = 1;
    return GR_OK;
}

int grafo_remover_aresta(Grafo *g, int a, int b) {
    int i = indice(g, a), j = indice(g, b);
    if (i < 0 || j < 0) return GR_VERTICE_INEXISTENTE;
    if (i == j || !g->adj[i][j]) return GR_ARESTA_INEXISTENTE;
    g->adj[i][j] = g->adj[j][i] = 0;
    return GR_OK;
}

int grafo_conectados(const Grafo *g, int a, int b, int *resultado) {
    int i = indice(g, a), j = indice(g, b);
    if (i < 0 || j < 0) return GR_VERTICE_INEXISTENTE;
    *resultado = (i != j && g->adj[i][j]);
    return GR_OK;
}

/* Indices dos vertices ativos ordenados por ID crescente (insercao, O(V^2)). */
static int ordenar_indices(const Grafo *g, int *ordem) {
    int m = 0;
    for (int i = 0; i < GRAFO_MAX; i++) {
        if (!g->ativo[i]) continue;
        int k = m++;
        while (k > 0 && g->id[ordem[k - 1]] > g->id[i]) { ordem[k] = ordem[k - 1]; k--; }
        ordem[k] = i;
    }
    return m;
}

int grafo_vizinhos(const Grafo *g, int id, int *saida, int cap) {
    int u = indice(g, id);
    if (u < 0) return GR_VERTICE_INEXISTENTE;
    int ordem[GRAFO_MAX];
    int m = ordenar_indices(g, ordem), n = 0;
    for (int k = 0; k < m && n < cap; k++)
        if (g->adj[u][ordem[k]]) saida[n++] = g->id[ordem[k]];
    return n;
}

int grafo_bfs(const Grafo *g, int origem, Lista *saida) {
    int o = indice(g, origem);
    if (o < 0) return GR_VERTICE_INEXISTENTE;

    int ordem[GRAFO_MAX];
    int m = ordenar_indices(g, ordem);          /* vizinhos visitados em ordem crescente de ID */
    char visitado[GRAFO_MAX] = {0};
    Fila f;
    fila_init(&f);

    visitado[o] = 1;                            /* marca ao entrar na fila */
    if (fila_enfileirar(&f, o) != 0) return GR_SEM_MEMORIA;

    int u;
    while (fila_desenfileirar(&f, &u) == 0) {
        if (lista_inserir_fim(saida, g->id[u]) != 0) { fila_liberar(&f); return GR_SEM_MEMORIA; }
        for (int k = 0; k < m; k++) {
            int v = ordem[k];
            if (g->adj[u][v] && !visitado[v]) {
                visitado[v] = 1;
                if (fila_enfileirar(&f, v) != 0) { fila_liberar(&f); return GR_SEM_MEMORIA; }
            }
        }
    }
    return GR_OK;
}

int grafo_existe_caminho(const Grafo *g, int a, int b, int *resultado) {
    if (indice(g, a) < 0 || indice(g, b) < 0) return GR_VERTICE_INEXISTENTE;
    Lista l;
    lista_init(&l);
    int rc = grafo_bfs(g, a, &l);
    if (rc == GR_OK) *resultado = lista_contem(&l, b);
    lista_liberar(&l);
    return rc;
}
