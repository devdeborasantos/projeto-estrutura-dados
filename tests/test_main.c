/* Testes reproduziveis (sem framework): make test
 * Cada linha imprime area, cenario, resultado esperado e obtido. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"
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

int main(void) {
    teste_bst();
    teste_lista_fila();
    teste_pilha();
    teste_ordenacao();
    printf("\n==== %d testes, %d falha(s) ====\n", total, falhas);
    return falhas ? 1 : 0;
}
