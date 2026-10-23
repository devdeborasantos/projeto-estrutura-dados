/* Testes reproduzíveis (sem framework): make test
 * Cada linha imprime area, cenario, resultado esperado e obtido. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entrada.h"
#include "sistema.h"
#include "ordenacao.h"
#include "fila.h"

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

static void cad(Sistema *s, int id, const char *titulo) {
    Livro l = mk(id, titulo, "Autor", 2000);
    sistema_cadastrar(s, &l);
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

/* ------------------------------------------------------------------ */
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

    /* degeneracao: IDs crescentes => altura = n */
    Bst d; bst_init(&d);
    for (int i = 1; i <= 10; i++) { Livro l = mk(i, "T", "A", 1); bst_inserir(&d, &l); }
    check_int("BST", "degeneracao (IDs crescentes): altura = n", 10, bst_altura(&d));
    bst_liberar(&d);

    Sistema s; sistema_init(&s);
    Livro l0 = mk(0, "T", "A", 1), ln = mk(-5, "T", "A", 1);
    check_int("BST", "ID zero invalido", SIS_ID_INVALIDO, sistema_cadastrar(&s, &l0));
    check_int("BST", "ID negativo invalido", SIS_ID_INVALIDO, sistema_cadastrar(&s, &ln));
    check_int("BST", "nada inserido com IDs invalidos", 0, s.bst.tamanho);
    sistema_liberar(&s);
}

static void teste_integracao(void) {
    Sistema s; sistema_init(&s);
    cad(&s, 10, "A"); cad(&s, 20, "B"); cad(&s, 30, "C");
    check_int("Integr.", "cadastro cria vertice", 1, grafo_existe(&s.grafo, 10));
    sistema_add_conexao(&s, 10, 20); sistema_add_conexao(&s, 10, 30);
    check_int("Integr.", "remocao: retorno", SIS_OK, sistema_remover(&s, 10));
    check_int("Integr.", "remocao: sumiu da BST", 0, bst_buscar(&s.bst, 10) != NULL);
    check_int("Integr.", "remocao: vertice removido", 0, grafo_existe(&s.grafo, 10));
    int r = 1;
    sistema_conexao_direta(&s, 20, 30, &r);
    check_int("Integr.", "remocao: demais vertices intactos", 3 - 1, grafo_num_vertices(&s.grafo));
    check_int("Integr.", "remover ID inexistente rejeitado", SIS_NAO_ENCONTRADO, sistema_remover(&s, 777));
    check_int("Integr.", "estado inalterado (BST=2)", 2, s.bst.tamanho);

    /* ID reaproveitado nao herda arestas antigas */
    cad(&s, 10, "A2");
    sistema_conexao_direta(&s, 10, 20, &r);
    check_int("Integr.", "recadastro de ID nao herda arestas antigas", 0, r);

    /* falha na criacao do vertice: rollback da BST */
    int antes = s.bst.tamanho;
    s.simular_falha_vertice = 1;
    Livro l = mk(55, "X", "Y", 1999);
    check_int("Integr.", "falha simulada de vertice => erro", SIS_ERRO_MEM, sistema_cadastrar(&s, &l));
    check_int("Integr.", "rollback: BST sem o registro", 0, bst_buscar(&s.bst, 55) != NULL);
    check_int("Integr.", "rollback: tamanho BST preservado", antes, s.bst.tamanho);
    check_int("Integr.", "rollback: sem vertice criado", 0, grafo_existe(&s.grafo, 55));

    /* falha na remocao: rollback restaura vertice e arestas */
    sistema_add_conexao(&s, 20, 30);
    sistema_add_conexao(&s, 20, 10);
    s.simular_falha_remocao_bst = 1;
    check_int("Integr.", "falha simulada na remocao => erro", SIS_ERRO_CONSISTENCIA, sistema_remover(&s, 20));
    check_int("Integr.", "rollback remocao: registro continua na BST", 1, bst_buscar(&s.bst, 20) != NULL);
    check_int("Integr.", "rollback remocao: vertice restaurado", 1, grafo_existe(&s.grafo, 20));
    sistema_conexao_direta(&s, 20, 30, &r);
    check_int("Integr.", "rollback remocao: aresta 20-30 restaurada", 1, r);
    sistema_conexao_direta(&s, 20, 10, &r);
    check_int("Integr.", "rollback remocao: aresta 20-10 restaurada", 1, r);
    sistema_liberar(&s);

    /* capacidade: 100 ok, 101o rejeitado */
    Sistema c; sistema_init(&c);
    int aceitos = 0;
    for (int i = 1; i <= 100; i++) { Livro x = mk(i, "T", "A", 1); aceitos += (sistema_cadastrar(&c, &x) == SIS_OK); }
    check_int("Integr.", "100 cadastros aceitos", 100, aceitos);
    Livro x101 = mk(101, "T", "A", 1);
    check_int("Integr.", "101o cadastro rejeitado (capacidade)", SIS_CAPACIDADE, sistema_cadastrar(&c, &x101));
    check_int("Integr.", "101o: sem registro parcial na BST", 0, bst_buscar(&c.bst, 101) != NULL);
    check_int("Integr.", "BST continua com 100", 100, c.bst.tamanho);
    sistema_remover(&c, 50);
    check_int("Integr.", "apos remover 1, novo cadastro cabe", SIS_OK, sistema_cadastrar(&c, &x101));
    sistema_liberar(&c);
}

static void teste_grafo(void) {
    Sistema s; sistema_init(&s);
    cad(&s, 1, "A"); cad(&s, 2, "B"); cad(&s, 3, "C"); cad(&s, 4, "D");
    int r = 0;
    check_int("Grafo", "aresta (1,2) criada", SIS_OK, sistema_add_conexao(&s, 1, 2));
    check_int("Grafo", "duplicata (2,1) rejeitada", SIS_CONEXAO_DUPLICADA, sistema_add_conexao(&s, 2, 1));
    check_int("Grafo", "duplicata (1,2) rejeitada", SIS_CONEXAO_DUPLICADA, sistema_add_conexao(&s, 1, 2));
    check_int("Grafo", "laco (3,3) rejeitado", SIS_LACO, sistema_add_conexao(&s, 3, 3));
    check_int("Grafo", "conexao com ID inexistente", SIS_NAO_ENCONTRADO, sistema_add_conexao(&s, 1, 99));
    check_int("Grafo", "remover aresta inexistente (3,4)", SIS_CONEXAO_INEXISTENTE, sistema_rem_conexao(&s, 3, 4));
    check_int("Grafo", "remover aresta com ID inexistente", SIS_NAO_ENCONTRADO, sistema_rem_conexao(&s, 3, 99));
    sistema_conexao_direta(&s, 2, 1, &r);
    check_int("Grafo", "conexao direta simetrica (2,1)", 1, r);
    check_int("Grafo", "conexao direta c/ ID inexistente", SIS_NAO_ENCONTRADO, sistema_conexao_direta(&s, 1, 99, &r));
    check_int("Grafo", "remover aresta (2,1) existente", SIS_OK, sistema_rem_conexao(&s, 2, 1));
    sistema_conexao_direta(&s, 1, 2, &r);
    check_int("Grafo", "aresta removida nos dois sentidos", 0, r);

    sistema_add_conexao(&s, 1, 2); sistema_add_conexao(&s, 1, 3); sistema_add_conexao(&s, 1, 4);
    check_int("Grafo", "remover vertice com varias arestas", SIS_OK, sistema_remover(&s, 1));
    check_int("Grafo", "vertice 2 sem aresta para 1", SIS_NAO_ENCONTRADO, sistema_conexao_direta(&s, 2, 1, &r));
    Lista l; lista_init(&l);
    sistema_alcancaveis(&s, 2, &l);
    check_int("Grafo", "2 ficou isolado (alcancaveis=1)", 1, l.tamanho);
    lista_liberar(&l);
    sistema_liberar(&s);
}

static void teste_bfs(void) {
    Sistema s; sistema_init(&s);
    int ordem_cad[] = {10, 5, 20, 1, 7, 99, 50, 51};
    for (int i = 0; i < 8; i++) cad(&s, ordem_cad[i], "T");
    sistema_add_conexao(&s, 10, 20); sistema_add_conexao(&s, 10, 5);
    sistema_add_conexao(&s, 10, 1);  sistema_add_conexao(&s, 5, 7);
    sistema_add_conexao(&s, 50, 51);
    char buf[512];
    Lista l; lista_init(&l);
    check_int("BFS", "origem existente", SIS_OK, sistema_alcancaveis(&s, 10, &l));
    lista_str(&l, buf);
    check_str("BFS", "ordem: vizinhos crescentes por ID, origem incluida", "10 1 5 20 7 ", buf);
    lista_liberar(&l);

    lista_init(&l);
    sistema_alcancaveis(&s, 99, &l);
    lista_str(&l, buf);
    check_str("BFS", "origem isolada produz so o proprio ID", "99 ", buf);
    lista_liberar(&l);

    lista_init(&l);
    sistema_alcancaveis(&s, 10, &l);
    check_int("BFS", "componente desconectado nao aparece (50)", 0, lista_contem(&l, 50));
    lista_liberar(&l);

    lista_init(&l);
    check_int("BFS", "origem inexistente => mensagem", SIS_NAO_ENCONTRADO, sistema_alcancaveis(&s, 12345, &l));
    check_int("BFS", "origem inexistente => lista vazia", 0, l.tamanho);
    lista_liberar(&l);

    int r = -1;
    sistema_caminho(&s, 10, 7, &r);
    check_int("Caminho", "existe caminho 10 -> 7", 1, r);
    sistema_caminho(&s, 7, 20, &r);
    check_int("Caminho", "existe caminho 7 -> 20 (indireto)", 1, r);
    sistema_caminho(&s, 10, 51, &r);
    check_int("Caminho", "nao existe caminho 10 -> 51", 0, r);
    check_int("Caminho", "ID ausente => mensagem, nao 'falso'", SIS_NAO_ENCONTRADO, sistema_caminho(&s, 10, 4242, &r));
    sistema_liberar(&s);

    /* lista: insercao no fim */
    Lista m; lista_init(&m);
    lista_inserir_fim(&m, 3); lista_inserir_fim(&m, 1); lista_inserir_fim(&m, 2);
    lista_str(&m, buf);
    check_str("Lista", "insercao no fim preserva ordem", "3 1 2 ", buf);
    lista_liberar(&m);

    /* fila FIFO */
    Fila f; fila_init(&f);
    int v = 0;
    check_int("Fila", "desenfileirar fila vazia", -1, fila_desenfileirar(&f, &v));
    fila_enfileirar(&f, 7); fila_enfileirar(&f, 8);
    fila_desenfileirar(&f, &v);
    check_int("Fila", "FIFO: primeiro a sair", 7, v);
    fila_liberar(&f);
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

    /* historico so de alteracoes bem-sucedidas */
    Sistema s; sistema_init(&s);
    cad(&s, 1, "A"); cad(&s, 2, "B");
    cad(&s, 1, "dup");                       /* falha */
    sistema_add_conexao(&s, 1, 2);           /* ok */
    sistema_add_conexao(&s, 1, 2);           /* falha */
    sistema_add_conexao(&s, 1, 1);           /* falha */
    sistema_rem_conexao(&s, 1, 2);           /* ok */
    sistema_remover(&s, 999);                /* falha */
    bst_buscar(&s.bst, 1);                   /* busca: fora do historico */
    Lista l; lista_init(&l); sistema_alcancaveis(&s, 1, &l); lista_liberar(&l);
    Livro *v; int n; sistema_ordenar(&s, &v, &n); free(v);
    sistema_remover(&s, 2);                  /* ok */
    check_int("Pilha", "historico: 2 cadastros + add + rem + remocao = 5", 5, s.historico.tamanho);
    Acao top; pilha_peek(&s.historico, &top);
    check_int("Pilha", "topo = remocao do ID 2", ACAO_REMOCAO, top.tipo);
    sistema_liberar(&s);
}

static void ordena_e_formata(Sistema *s, char *buf) {
    Livro *v; int n;
    sistema_ordenar(s, &v, &n);
    buf[0] = '\0';
    for (int i = 0; i < n; i++) {
        size_t k = strlen(buf);
        snprintf(buf + k, 512 - k, "%s#%d ", v[i].titulo, v[i].id);
    }
    printf("       saida exibida: %s\n", buf);
    free(v);
}

static void teste_ordenacao(void) {
    char buf[512], antes[512], depois[512];
    Sistema s;

    sistema_init(&s);
    cad(&s, 1, "Banana"); cad(&s, 2, "Abacaxi"); cad(&s, 3, "Cereja");
    ids_em_ordem(&s.bst, antes);
    ordena_e_formata(&s, buf);
    check_str("Ordenacao", "vetor desordenado", "Abacaxi#2 Banana#1 Cereja#3 ", buf);
    ids_em_ordem(&s.bst, depois);
    check_str("Ordenacao", "BST intacta (ordem de IDs igual)", antes, depois);
    check_int("Ordenacao", "BST intacta (tamanho)", 3, s.bst.tamanho);
    sistema_liberar(&s);

    sistema_init(&s);
    cad(&s, 1, "A"); cad(&s, 2, "B"); cad(&s, 3, "C");
    ordena_e_formata(&s, buf);
    check_str("Ordenacao", "vetor ja ordenado", "A#1 B#2 C#3 ", buf);
    sistema_liberar(&s);

    sistema_init(&s);
    cad(&s, 1, "C"); cad(&s, 2, "B"); cad(&s, 3, "A");
    ordena_e_formata(&s, buf);
    check_str("Ordenacao", "vetor invertido", "A#3 B#2 C#1 ", buf);
    sistema_liberar(&s);

    sistema_init(&s);
    cad(&s, 30, "Mesmo"); cad(&s, 10, "Mesmo"); cad(&s, 20, "Mesmo");
    ordena_e_formata(&s, buf);
    check_str("Ordenacao", "empate textual: desempate por ID", "Mesmo#10 Mesmo#20 Mesmo#30 ", buf);
    sistema_liberar(&s);

    sistema_init(&s);
    cad(&s, 1, "abc"); cad(&s, 2, "Abc"); cad(&s, 3, "ABC");
    ordena_e_formata(&s, buf);
    check_str("Ordenacao", "maiusculas antes de minusculas (strcmp)", "ABC#3 Abc#2 abc#1 ", buf);
    sistema_liberar(&s);

    sistema_init(&s);
    Livro *v; int n = -1;
    check_int("Ordenacao", "base vazia: retorno ok", SIS_OK, sistema_ordenar(&s, &v, &n));
    check_int("Ordenacao", "base vazia: n = 0", 0, n);
    sistema_liberar(&s);
}

static FILE *fonte(const char *conteudo) {
    FILE *f = tmpfile();
    fputs(conteudo, f);
    rewind(f);
    return f;
}

static void teste_entradas(void) {
    char dest[TAM_TEXTO], longo[400];
    int v = 0;
    FILE *f;

    f = fonte("\n");
    check_int("Entradas", "texto vazio", ENT_VAZIO, ent_ler_texto(f, dest, sizeof dest));
    fclose(f);
    f = fonte("     \n");
    check_int("Entradas", "texto so com espacos", ENT_VAZIO, ent_ler_texto(f, dest, sizeof dest));
    fclose(f);
    f = fonte("  Dom Casmurro  \n");
    check_int("Entradas", "texto com espacos internos: aceito", ENT_OK, ent_ler_texto(f, dest, sizeof dest));
    check_str("Entradas", "texto com espacos: aparado", "Dom Casmurro", dest);
    fclose(f);

    memset(longo, 'x', 79); longo[79] = '\n'; longo[80] = '\0';
    f = fonte(longo);
    check_int("Entradas", "texto com exatamente 79 chars", ENT_OK, ent_ler_texto(f, dest, sizeof dest));
    fclose(f);
    memset(longo, 'x', 80); longo[80] = '\n'; longo[81] = '\0';
    f = fonte(longo);
    check_int("Entradas", "texto com 80 chars rejeitado", ENT_LONGO, ent_ler_texto(f, dest, sizeof dest));
    fclose(f);

    /* limpeza de buffer: linha enorme nao contamina a leitura seguinte */
    memset(longo, 'y', 399); longo[399] = '\0';
    char conteudo[600];
    snprintf(conteudo, sizeof conteudo, "%s\nproximo\n42\n", longo);
    f = fonte(conteudo);
    check_int("Entradas", "linha de 399 chars rejeitada", ENT_LONGO, ent_ler_texto(f, dest, sizeof dest));
    check_int("Entradas", "limpeza de buffer: leitura seguinte ok", ENT_OK, ent_ler_texto(f, dest, sizeof dest));
    check_str("Entradas", "limpeza de buffer: conteudo correto", "proximo", dest);
    check_int("Entradas", "inteiro lido depois", ENT_OK, ent_ler_int(f, 1, 100, &v));
    check_int("Entradas", "valor inteiro", 42, v);
    check_int("Entradas", "fim da entrada", ENT_EOF, ent_ler_int(f, 1, 100, &v));
    fclose(f);

    struct { const char *txt; int esp; const char *desc; } casos[] = {
        {"abc\n", ENT_INVALIDO, "numero invalido: letras"},
        {"12x\n", ENT_INVALIDO, "numero invalido: sufixo"},
        {"1.5\n", ENT_INVALIDO, "numero invalido: decimal"},
        {"\n", ENT_VAZIO, "numero vazio"},
        {"0\n", ENT_FORA_INTERVALO, "ID zero"},
        {"-3\n", ENT_FORA_INTERVALO, "ID negativo"},
        {"99999999999999999999999\n", ENT_OVERFLOW, "overflow (> long)"},
        {"4294967296\n", ENT_OVERFLOW, "overflow (> int)"},
        {"5000\n", ENT_OK, "ID valido 5000"},
    };
    for (size_t i = 0; i < sizeof casos / sizeof *casos; i++) {
        f = fonte(casos[i].txt);
        check_int("Entradas", casos[i].desc, casos[i].esp, ent_ler_int(f, 1, 2147483647L, &v));
        fclose(f);
    }

    f = fonte("0\n");     check_int("Entradas", "ano 0 fora do intervalo", ENT_FORA_INTERVALO, ent_ler_int(f, ANO_MIN, ANO_MAX, &v)); fclose(f);
    f = fonte("2027\n");  check_int("Entradas", "ano 2027 fora do intervalo", ENT_FORA_INTERVALO, ent_ler_int(f, ANO_MIN, ANO_MAX, &v)); fclose(f);
    f = fonte("2026\n");  check_int("Entradas", "ano 2026 valido", ENT_OK, ent_ler_int(f, ANO_MIN, ANO_MAX, &v)); fclose(f);
    f = fonte("1\n");     check_int("Entradas", "ano 1 valido", ENT_OK, ent_ler_int(f, ANO_MIN, ANO_MAX, &v)); fclose(f);

    Sistema s; sistema_init(&s);
    Livro l = mk(1, "T", "A", 2027);
    check_int("Entradas", "cadastro com ano 2027 rejeitado", SIS_ANO_INVALIDO, sistema_cadastrar(&s, &l));
    l = mk(1, "   ", "A", 2000);
    check_int("Entradas", "cadastro com titulo em branco rejeitado", SIS_CAMPO_INVALIDO, sistema_cadastrar(&s, &l));
    check_int("Entradas", "nenhum registro parcial", 0, s.bst.tamanho);
    sistema_liberar(&s);
}

int main(void) {
    teste_bst();
    teste_integracao();
    teste_grafo();
    teste_bfs();
    teste_pilha();
    teste_ordenacao();
    teste_entradas();
    printf("\n==== %d testes, %d falha(s) ====\n", total, falhas);
    return falhas ? 1 : 0;
}
