#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "entrada.h"
#include "sistema.h"
#include "ordenacao.h"

/* ---------- leitura com repeticao ate entrada valida (-1 = fim da entrada) ---------- */
static int pedir_texto(const char *prompt, char *dest) {
    for (;;) {
        printf("%s", prompt);
        fflush(stdout);
        int rc = ent_ler_texto(stdin, dest, TAM_TEXTO);
        if (rc == ENT_OK) return 0;
        if (rc == ENT_EOF) return -1;
        printf("Entrada invalida: %s\n", ent_mensagem(rc));
    }
}

static int pedir_int(const char *prompt, long min, long max, int *out) {
    for (;;) {
        printf("%s", prompt);
        fflush(stdout);
        int rc = ent_ler_int(stdin, min, max, out);
        if (rc == ENT_OK) return 0;
        if (rc == ENT_EOF) return -1;
        printf("Entrada invalida: %s\n", ent_mensagem(rc));
    }
}

#define PEDIR_ID(prompt, var) do { if (pedir_int(prompt, 1, INT_MAX, &(var)) != 0) return -1; } while (0)

/* ---------- apresentacao ---------- */
static void mostrar_livro(const Livro *l) {
    printf("ID %d | Titulo: %s | Autor: %s | Ano: %d\n", l->id, l->titulo, l->autor, l->ano);
}
static void mostrar_cb(const Livro *l, void *ctx) { (void)ctx; mostrar_livro(l); }

static void menu(void) {
    puts("============================================");
    puts("       SISTEMA DE ESTRUTURA DE DADOS");
    puts("            (Tema: Biblioteca)");
    puts("============================================");
    puts(" 1 - Cadastrar registro (cria vertice)");
    puts(" 2 - Buscar registro (BST)");
    puts(" 3 - Remover registro e vertice/arestas");
    puts(" 4 - Listar registros (BST em-ordem)");
    puts(" 5 - Ordenar e exibir registros por titulo");
    puts(" 6 - Adicionar conexao (aresta)");
    puts(" 7 - Remover conexao (aresta)");
    puts(" 8 - Consultar conexao direta");
    puts(" 9 - Listar alcancaveis (BFS + lista)");
    puts("10 - Verificar caminho entre dois IDs");
    puts("11 - Consultar historico (pilha)");
    puts(" 0 - Encerrar");
    puts("============================================");
}

/* ---------- operacoes (retornam -1 apenas em fim de entrada) ---------- */
static int op_cadastrar(Sistema *s) {
    Livro l;
    PEDIR_ID("ID: ", l.id);
    if (bst_buscar(&s->bst, l.id)) { puts(sistema_mensagem(SIS_ID_DUPLICADO)); return 0; }
    if (pedir_texto("Titulo: ", l.titulo) != 0) return -1;
    if (pedir_texto("Autor: ", l.autor) != 0) return -1;
    if (pedir_int("Ano de publicacao (1 a 2026): ", ANO_MIN, ANO_MAX, &l.ano) != 0) return -1;
    puts(sistema_mensagem(sistema_cadastrar(s, &l)));
    return 0;
}

static int op_buscar(Sistema *s) {
    int id;
    PEDIR_ID("ID a buscar: ", id);
    const Livro *l = bst_buscar(&s->bst, id);
    if (l) mostrar_livro(l); else puts(sistema_mensagem(SIS_NAO_ENCONTRADO));
    return 0;
}

static int op_remover(Sistema *s) {
    int id;
    PEDIR_ID("ID a remover: ", id);
    puts(sistema_mensagem(sistema_remover(s, id)));
    return 0;
}

static void op_listar(Sistema *s) {
    if (s->bst.tamanho == 0) { puts("Nenhum registro cadastrado."); return; }
    bst_em_ordem(&s->bst, mostrar_cb, NULL);
    printf("Total: %d | Altura da BST: %d\n", s->bst.tamanho, bst_altura(&s->bst));
    printf("Menor ID: %d | Maior ID: %d\n", bst_menor(&s->bst)->id, bst_maior(&s->bst)->id);
}

static void op_ordenar(Sistema *s) {
    Livro *v;
    int n;
    int rc = sistema_ordenar(s, &v, &n);
    if (rc != SIS_OK) { puts(sistema_mensagem(rc)); return; }
    if (n == 0) { puts("Nenhum registro cadastrado."); return; }
    for (int i = 0; i < n; i++) mostrar_livro(&v[i]);
    free(v);                                         /* libera vetor auxiliar */
}

static int pedir_par(int *a, int *b) {
    PEDIR_ID("ID A: ", *a);
    PEDIR_ID("ID B: ", *b);
    return 0;
}

static int op_add_conexao(Sistema *s) {
    int a, b;
    if (pedir_par(&a, &b)) return -1;
    puts(sistema_mensagem(sistema_add_conexao(s, a, b)));
    return 0;
}

static int op_rem_conexao(Sistema *s) {
    int a, b;
    if (pedir_par(&a, &b)) return -1;
    puts(sistema_mensagem(sistema_rem_conexao(s, a, b)));
    return 0;
}

static int op_conexao_direta(Sistema *s) {
    int a, b, res = 0;
    if (pedir_par(&a, &b)) return -1;
    int rc = sistema_conexao_direta(s, a, b, &res);
    if (rc != SIS_OK) puts(sistema_mensagem(rc));
    else printf(res ? "Existe conexao direta entre %d e %d.\n" : "Nao existe conexao direta entre %d e %d.\n", a, b);
    return 0;
}

static int op_alcancaveis(Sistema *s) {
    int o;
    PEDIR_ID("ID de origem: ", o);
    Lista l;
    lista_init(&l);
    int rc = sistema_alcancaveis(s, o, &l);
    if (rc != SIS_OK) puts(sistema_mensagem(rc));
    else {
        printf("Alcancaveis a partir de %d (%d): ", o, l.tamanho);
        lista_imprimir(&l);
    }
    lista_liberar(&l);
    return 0;
}

static int op_caminho(Sistema *s) {
    int a, b, res = 0;
    if (pedir_par(&a, &b)) return -1;
    int rc = sistema_caminho(s, a, b, &res);
    if (rc != SIS_OK) puts(sistema_mensagem(rc));
    else printf(res ? "Existe caminho entre %d e %d.\n" : "Nao existe caminho entre %d e %d.\n", a, b);
    return 0;
}

int main(void) {
    Sistema s;
    sistema_init(&s);

    for (;;) {
        menu();
        int op;
        if (pedir_int("Escolha uma opcao: ", 0, 11, &op) != 0) break;   /* EOF: encerra */
        int rc = 0;
        switch (op) {
            case 1:  rc = op_cadastrar(&s); break;
            case 2:  rc = op_buscar(&s); break;
            case 3:  rc = op_remover(&s); break;
            case 4:  op_listar(&s); break;
            case 5:  op_ordenar(&s); break;
            case 6:  rc = op_add_conexao(&s); break;
            case 7:  rc = op_rem_conexao(&s); break;
            case 8:  rc = op_conexao_direta(&s); break;
            case 9:  rc = op_alcancaveis(&s); break;
            case 10: rc = op_caminho(&s); break;
            case 11: pilha_imprimir_ultimas(&s.historico, 5); break;
            case 0:  sistema_liberar(&s); puts("Encerrado. Memoria liberada."); return 0;
        }
        if (rc != 0) break;
        puts("");
    }
    sistema_liberar(&s);
    puts("\nEntrada encerrada. Memoria liberada.");
    return 0;
}
