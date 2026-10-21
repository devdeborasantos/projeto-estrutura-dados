/* main.c PROVISORIO (fase 1): so valida a leitura segura de entradas.
 * Sera substituido pelo menu completo na fase 5. */
#include <stdio.h>
#include "entrada.h"

int main(void) {
    for (;;) {
        puts("==== SISTEMA DE ESTRUTURA DE DADOS (em construcao) ====");
        puts(" 0 - Encerrar");
        printf("Escolha uma opcao: ");
        fflush(stdout);
        int op;
        int rc = ent_ler_int(stdin, 0, 11, &op);
        if (rc == ENT_EOF) break;
        if (rc != ENT_OK) { printf("Entrada invalida: %s\n\n", ent_mensagem(rc)); continue; }
        if (op == 0) break;
        puts("Opcao ainda nao implementada.\n");
    }
    puts("Encerrado.");
    return 0;
}
