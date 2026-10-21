#ifndef ENTRADA_H
#define ENTRADA_H

#include <stdio.h>
#include <stddef.h>

/* Leitura segura de entradas (fgets + strtol). Recebe FILE* para permitir testes. */
enum {
    ENT_OK = 0,
    ENT_EOF = -1,
    ENT_VAZIO = -2,
    ENT_LONGO = -3,
    ENT_INVALIDO = -4,
    ENT_OVERFLOW = -5,
    ENT_FORA_INTERVALO = -6
};

/* Le uma linha de texto (aceita espacos). Rejeita vazio/so espacos e > 79 caracteres.
 * Em linha longa, descarta o restante da linha. dest deve ter tam_dest >= 80. */
int ent_ler_texto(FILE *f, char *dest, size_t tam_dest);

/* Le um inteiro validado com strtol, dentro de [min, max]. */
int ent_ler_int(FILE *f, long min, long max, int *out);

const char *ent_mensagem(int codigo);

#endif
