/* Testes reproduziveis (sem framework): make test
 * Cada linha imprime area, cenario, resultado esperado e obtido. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"
#include "grafo.h"
#include "lista.h"
#include "fila.h"
#include "pilha.h"
#include "ordenacao.h"

static int total = 0, falhas = 0;

static void check_int(const char *area, const char *desc, long esp, long obt) {
    total++;
    if (esp == obt) printf("[PASS] %-10s %-58s esperado=%ld obtido=%ld\n", area, desc, esp, obt);
    else { falhas++; printf("[FAIL] %-10s %-58s esperado=%ld obtido=%ld\n", area, desc, esp, obt); }
}
static void check_str(const char *area, const char *desc, const char *esp, const char *obt) {
    total++;
    if (strcmp(esp, obt) == 0) printf("[PASS] %-10s %-58s esperado=\"%s\" obtido=\"%s\"\n", area, desc, esp, obt);
    else { falhas++; printf("[FAIL] %-10s %-58s esperado=\"%s\" obtido=\"%s\"\n", area, desc, esp, obt); }
}

static Livro mk(int id, const char *t, const char *a, int ano) {
    Livro l;
    memset(&l, 0, sizeof l);
    l.id = id;
    snprintf(l.titulo, TAM_TEXTO, "%s", t);
    snprintf(l.autor, TAM_TEXTO, "%s", a);
    l.ano = ano;
    return l;
}

static void cb_ids(const Livro *l, void *ctx) {
    char *buf = ctx;
    size_t n = strlen(buf);
    snprintf(buf + n, 512 - n, "%d ", l->id);
}
static void ids_em_ordem(const Bst *b, char *buf) { buf[0] = '\0'; bst_em_ordem(b, cb_ids, buf); }

static void lista_str(const Lista *l, char *buf) {
    buf[0] = '\0';
    for (const NoLista *p = l->ini; p; p = p->prox) {
        size_t n = strlen(buf);
        snprintf(buf + n, 512 - n, "%d ", p->valor);
    }
}

/* ------------------------------ BST ------------------------------ */
static void teste_bst(void) {
    Bst b; bst_init(&b);
    char buf[512];
    int ids[] = {50, 30, 70, 20, 40, 60, 80};
    int ok = 0;
    for (int i = 0; i < 7; i++) { Livro l = mk(ids[i], "T", "A", 1); ok += (bst_inserir(&b, &l) == BST_OK); }
    check_int("BST", "inserir 7 IDs distintos", 7, ok);
    Livro dup = mk(50, "T", "A", 1);
    check_int("BST", "ID duplicado rejeitado", BST_DUPLICADO, bst_inserir(&b, &dup));
    check_int("BST", "tamanho apos duplicado", 7, b.tamanho);
    ids_em_ordem(&b, buf);
    check_str("BST", "em-ordem", "20 30 40 50 60 70 80 ", buf);
    check_int("BST", "busca existente (60)", 1, bst_buscar(&b, 60) != NULL);
    check_int("BST", "busca ausente (65)", 0, bst_buscar(&b, 65) != NULL);
    check_int("BST", "menor ID", 20, bst_menor(&b)->id);
    check_int("BST", "maior ID", 80, bst_maior(&b)->id);
    check_int("BST", "remover ID inexistente (999)", BST_NAO_ENCONTRADO, bst_remover(&b, 999));
    check_int("BST", "tamanho inalterado apos remocao invalida", 7, b.tamanho);
    bst_remover(&b, 20);
    ids_em_ordem(&b, buf);
    check_str("BST", "remover folha (20)", "30 40 50 60 70 80 ", buf);
    bst_remover(&b, 30);
    ids_em_ordem(&b, buf);
    check_str("BST", "remover no com 1 filho (30)", "40 50 60 70 80 ", buf);
    bst_remover(&b, 70);
    ids_em_ordem(&b, buf);
    check_str("BST", "remover no com 2 filhos (70)", "40 50 60 80 ", buf);
    bst_remover(&b, 50);
    ids_em_ordem(&b, buf);
    check_str("BST", "remover raiz com 2 filhos (50)", "40 60 80 ", buf);
    check_int("BST", "tamanho final", 3, b.tamanho);
    bst_liberar(&b);

    Bst v; bst_init(&v);
    check_int("BST", "arvore vazia: menor = NULL", 1, bst_menor(&v) == NULL);
    check_int("BST", "arvore vazia: remover => nao encontrado", BST_NAO_ENCONTRADO, bst_remover(&v, 1));

    /* degeneracao: IDs crescentes => altura = n */
    Bst d; bst_init(&d);
    for (int i = 1; i <= 10; i++) { Livro l = mk(i, "T", "A", 1); bst_inserir(&d, &l); }
    check_int("BST", "degeneracao (IDs crescentes): altura = n", 10, bst_altura(&d));
    bst_liberar(&d);
}

/* ------------------- Lista, Fila, Pilha, Ordenacao ------------------- */
static void teste_lista_fila(void) {
    char buf[512];
    Lista m; lista_init(&m);
    lista_inserir_fim(&m, 3); lista_inserir_fim(&m, 1); lista_inserir_fim(&m, 2);
    lista_str(&m, buf);
    check_str("Lista", "insercao no fim preserva ordem", "3 1 2 ", buf);
    check_int("Lista", "tamanho", 3, m.tamanho);
    check_int("Lista", "contem 1", 1, lista_contem(&m, 1));
    check_int("Lista", "nao contem 9", 0, lista_contem(&m, 9));
    lista_liberar(&m);
    check_int("Lista", "apos liberar: tamanho 0", 0, m.tamanho);

    Fila f; fila_init(&f);
    int v = 0;
    check_int("Fila", "desenfileirar fila vazia", -1, fila_desenfileirar(&f, &v));
    fila_enfileirar(&f, 7); fila_enfileirar(&f, 8); fila_enfileirar(&f, 9);
    fila_desenfileirar(&f, &v);
    check_int("Fila", "FIFO: primeiro a sair", 7, v);
    fila_desenfileirar(&f, &v);
    check_int("Fila", "FIFO: segundo a sair", 8, v);
    check_int("Fila", "fila nao vazia", 0, fila_vazia(&f));
    fila_liberar(&f);
    check_int("Fila", "apos liberar: vazia", 1, fila_vazia(&f));
}

static void teste_pilha(void) {
    Pilha p; pilha_init(&p);
    Acao a = {ACAO_CADASTRO, 1, 0}, saida[5];
    check_int("Pilha", "pop em pilha vazia", -1, pilha_pop(&p, &a));
    check_int("Pilha", "peek em pilha vazia", -1, pilha_peek(&p, &a));
    check_int("Pilha", "consulta em pilha vazia", 0, pilha_ultimas(&p, 5, saida));
    for (int i = 1; i <= 3; i++) { Acao x = {ACAO_CADASTRO, i, 0}; pilha_push(&p, x); }
    check_int("Pilha", "menos de cinco: retorna 3", 3, pilha_ultimas(&p, 5, saida));
    check_int("Pilha", "mais recente primeiro", 3, saida[0].id1);
    for (int i = 4; i <= 9; i++) { Acao x = {ACAO_CADASTRO, i, 0}; pilha_push(&p, x); }
    check_int("Pilha", "mais de cinco: limita a 5", 5, pilha_ultimas(&p, 5, saida));
    check_int("Pilha", "mais de cinco: topo = 9", 9, saida[0].id1);
    check_int("Pilha", "mais de cinco: quinta = 5", 5, saida[4].id1);
    check_int("Pilha", "consulta nao destrutiva (tamanho 9)", 9, p.tamanho);
    pilha_peek(&p, &a);
    check_int("Pilha", "peek devolve topo sem remover", 9, a.id1);
    check_int("Pilha", "tamanho apos peek", 9, p.tamanho);
    pilha_pop(&p, &a);
    check_int("Pilha", "pop devolve topo", 9, a.id1);
    check_int("Pilha", "tamanho apos pop", 8, p.tamanho);
    printf("       (saida de pilha_imprimir_ultimas:)\n");
    pilha_imprimir_ultimas(&p, 5);
    pilha_liberar(&p);
}

static void formatar(const Livro *v, int n, char *buf) {
    buf[0] = '\0';
    for (int i = 0; i < n; i++) {
        size_t k = strlen(buf);
        snprintf(buf + k, 512 - k, "%s#%d ", v[i].titulo, v[i].id);
    }
    printf("       saida exibida: %s\n", buf);
}

static void teste_ordenacao(void) {
    char buf[512];
    Livro v1[] = { mk(1, "Banana", "A", 1), mk(2, "Abacaxi", "A", 1), mk(3, "Cereja", "A", 1) };
    ordenar_livros(v1, 3); formatar(v1, 3, buf);
    check_str("Ordenacao", "vetor desordenado", "Abacaxi#2 Banana#1 Cereja#3 ", buf);

    Livro v2[] = { mk(1, "A", "A", 1), mk(2, "B", "A", 1), mk(3, "C", "A", 1) };
    ordenar_livros(v2, 3); formatar(v2, 3, buf);
    check_str("Ordenacao", "vetor ja ordenado", "A#1 B#2 C#3 ", buf);

    Livro v3[] = { mk(1, "C", "A", 1), mk(2, "B", "A", 1), mk(3, "A", "A", 1) };
    ordenar_livros(v3, 3); formatar(v3, 3, buf);
    check_str("Ordenacao", "vetor invertido", "A#3 B#2 C#1 ", buf);

    Livro v4[] = { mk(30, "Mesmo", "A", 1), mk(10, "Mesmo", "A", 1), mk(20, "Mesmo", "A", 1) };
    ordenar_livros(v4, 3); formatar(v4, 3, buf);
    check_str("Ordenacao", "empate textual: desempate por ID", "Mesmo#10 Mesmo#20 Mesmo#30 ", buf);

    Livro v5[] = { mk(1, "abc", "A", 1), mk(2, "Abc", "A", 1), mk(3, "ABC", "A", 1) };
    ordenar_livros(v5, 3); formatar(v5, 3, buf);
    check_str("Ordenacao", "maiusculas antes de minusculas (strcmp)", "ABC#3 Abc#2 abc#1 ", buf);

    Livro v6[] = { mk(1, "X", "A", 1) };
    ordenar_livros(v6, 1);
    check_int("Ordenacao", "vetor com 1 elemento", 1, v6[0].id);
    ordenar_livros(NULL, 0);
    check_int("Ordenacao", "vetor vazio nao quebra", 1, 1);

    /* BST intacta: ordena uma COPIA e confere que a BST nao mudou */
    Bst b; bst_init(&b);
    int ids[] = {5, 2, 8};
    const char *tit[] = {"Zeta", "Alfa", "Mu"};
    for (int i = 0; i < 3; i++) { Livro l = mk(ids[i], tit[i], "A", 1); bst_inserir(&b, &l); }
    char antes[512], depois[512];
    ids_em_ordem(&b, antes);
    Livro cop[3];
    int n = bst_copiar(&b, cop, 3);
    ordenar_livros(cop, n); formatar(cop, n, buf);
    check_str("Ordenacao", "copia da BST ordenada por titulo", "Alfa#2 Mu#8 Zeta#5 ", buf);
    ids_em_ordem(&b, depois);
    check_str("Ordenacao", "BST intacta (ordem de IDs igual)", antes, depois);
    check_int("Ordenacao", "BST intacta (tamanho)", 3, b.tamanho);
    bst_liberar(&b);
}

/* ------------------------------ Grafo e BFS ------------------------------ */
static void teste_grafo(void) {
    Grafo g; grafo_init(&g);
    int r = 0;
    for (int i = 1; i <= 4; i++) grafo_criar_vertice(&g, i);
    check_int("Grafo", "4 vertices criados", 4, grafo_num_vertices(&g));
    check_int("Grafo", "vertice duplicado rejeitado", GR_VERTICE_DUPLICADO, grafo_criar_vertice(&g, 2));
    check_int("Grafo", "ID zero invalido", GR_ID_INVALIDO, grafo_criar_vertice(&g, 0));
    check_int("Grafo", "aresta (1,2) criada", GR_OK, grafo_add_aresta(&g, 1, 2));
    check_int("Grafo", "duplicata (2,1) rejeitada", GR_ARESTA_DUPLICADA, grafo_add_aresta(&g, 2, 1));
    check_int("Grafo", "duplicata (1,2) rejeitada", GR_ARESTA_DUPLICADA, grafo_add_aresta(&g, 1, 2));
    check_int("Grafo", "laco (3,3) rejeitado", GR_LACO, grafo_add_aresta(&g, 3, 3));
    check_int("Grafo", "conexao com ID inexistente", GR_VERTICE_INEXISTENTE, grafo_add_aresta(&g, 1, 99));
    check_int("Grafo", "remover aresta inexistente (3,4)", GR_ARESTA_INEXISTENTE, grafo_remover_aresta(&g, 3, 4));
    check_int("Grafo", "remover aresta com ID inexistente", GR_VERTICE_INEXISTENTE, grafo_remover_aresta(&g, 3, 99));
    grafo_conectados(&g, 2, 1, &r);
    check_int("Grafo", "conexao direta simetrica (2,1)", 1, r);
    check_int("Grafo", "conexao direta c/ ID inexistente", GR_VERTICE_INEXISTENTE, grafo_conectados(&g, 1, 99, &r));
    check_int("Grafo", "remover aresta (2,1)", GR_OK, grafo_remover_aresta(&g, 2, 1));
    grafo_conectados(&g, 1, 2, &r);
    check_int("Grafo", "aresta removida nos dois sentidos", 0, r);

    grafo_add_aresta(&g, 1, 2); grafo_add_aresta(&g, 1, 3); grafo_add_aresta(&g, 1, 4);
    check_int("Grafo", "remover vertice com varias arestas", GR_OK, grafo_remover_vertice(&g, 1));
    check_int("Grafo", "vertice 1 nao existe mais", 0, grafo_existe(&g, 1));
    check_int("Grafo", "restam 3 vertices", 3, grafo_num_vertices(&g));
    check_int("Grafo", "arestas incidentes removidas", GR_VERTICE_INEXISTENTE, grafo_conectados(&g, 2, 1, &r));
    check_int("Grafo", "remover vertice inexistente", GR_VERTICE_INEXISTENTE, grafo_remover_vertice(&g, 1));

    grafo_criar_vertice(&g, 1);
    grafo_conectados(&g, 1, 2, &r);
    check_int("Grafo", "ID recriado nao herda arestas antigas", 0, r);

    /* capacidade fixa de 100 */
    Grafo c; grafo_init(&c);
    int ok = 0;
    for (int i = 1; i <= 100; i++) ok += (grafo_criar_vertice(&c, i * 7) == GR_OK);   /* IDs arbitrarios */
    check_int("Grafo", "100 vertices com IDs arbitrarios", 100, ok);
    check_int("Grafo", "101o vertice rejeitado (cheio)", GR_CHEIO, grafo_criar_vertice(&c, 5000));
    check_int("Grafo", "continua com 100", 100, grafo_num_vertices(&c));
    grafo_remover_vertice(&c, 70);
    check_int("Grafo", "apos remover 1, slot reaproveitado", GR_OK, grafo_criar_vertice(&c, 5000));
}

static void teste_bfs(void) {
    Grafo g; grafo_init(&g);
    int ordem_cad[] = {10, 5, 20, 1, 7, 99, 50, 51};
    for (int i = 0; i < 8; i++) grafo_criar_vertice(&g, ordem_cad[i]);
    grafo_add_aresta(&g, 10, 20); grafo_add_aresta(&g, 10, 5);
    grafo_add_aresta(&g, 10, 1);  grafo_add_aresta(&g, 5, 7);
    grafo_add_aresta(&g, 50, 51);

    char buf[512];
    int viz[GRAFO_MAX];
    int nv = grafo_vizinhos(&g, 10, viz, GRAFO_MAX);
    check_int("BFS", "vizinhos de 10: quantidade", 3, nv);
    check_int("BFS", "vizinhos em ordem crescente (1,5,20)", 1 * 10000 + 5 * 100 + 20, viz[0] * 10000 + viz[1] * 100 + viz[2]);

    Lista l; lista_init(&l);
    check_int("BFS", "origem existente", GR_OK, grafo_bfs(&g, 10, &l));
    lista_str(&l, buf);
    check_str("BFS", "ordem: vizinhos crescentes por ID, origem incluida", "10 1 5 20 7 ", buf);
    check_int("BFS", "componente desconectado nao aparece (50)", 0, lista_contem(&l, 50));
    lista_liberar(&l);

    lista_init(&l);
    grafo_bfs(&g, 99, &l);
    lista_str(&l, buf);
    check_str("BFS", "origem isolada produz so o proprio ID", "99 ", buf);
    lista_liberar(&l);

    lista_init(&l);
    check_int("BFS", "origem inexistente => erro", GR_VERTICE_INEXISTENTE, grafo_bfs(&g, 12345, &l));
    check_int("BFS", "origem inexistente => lista vazia", 0, l.tamanho);
    lista_liberar(&l);

    int r = -1;
    grafo_existe_caminho(&g, 10, 7, &r);
    check_int("Caminho", "existe caminho 10 -> 7", 1, r);
    grafo_existe_caminho(&g, 7, 20, &r);
    check_int("Caminho", "existe caminho 7 -> 20 (indireto)", 1, r);
    grafo_existe_caminho(&g, 10, 51, &r);
    check_int("Caminho", "nao existe caminho 10 -> 51", 0, r);
    check_int("Caminho", "ID ausente => erro, nao 'falso'", GR_VERTICE_INEXISTENTE, grafo_existe_caminho(&g, 10, 4242, &r));
}

int main(void) {
    teste_bst();
    teste_lista_fila();
    teste_pilha();
    teste_ordenacao();
    teste_grafo();
    teste_bfs();
    printf("\n==== %d testes, %d falha(s) ====\n", total, falhas);
    return falhas ? 1 : 0;
}
