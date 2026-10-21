#include "entrada.h"
#include "registro.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define TAM_LINHA 256

static void descartar_resto(FILE *f) {
    int c;
    while ((c = fgetc(f)) != '\n' && c != EOF) { }
}

static int ler_linha(FILE *f, char *buf, size_t tam) {
    if (!fgets(buf, (int)tam, f)) return ENT_EOF;
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[--len] = '\0';
    } else if (!feof(f)) {            /* linha maior que o buffer */
        descartar_resto(f);
        return ENT_LONGO;
    }
    if (len > 0 && buf[len - 1] == '\r') buf[--len] = '\0';
    return ENT_OK;
}

int ent_ler_texto(FILE *f, char *dest, size_t tam_dest) {
    char linha[TAM_LINHA];
    int rc = ler_linha(f, linha, sizeof linha);
    if (rc != ENT_OK) return rc;

    size_t len = strlen(linha);
    if (len > MAX_UTIL || len >= tam_dest) return ENT_LONGO;

    size_t ini = 0, fim = len;
    while (ini < len && isspace((unsigned char)linha[ini])) ini++;
    if (ini == len) return ENT_VAZIO;
    while (fim > ini && isspace((unsigned char)linha[fim - 1])) fim--;

    memcpy(dest, linha + ini, fim - ini);
    dest[fim - ini] = '\0';
    return ENT_OK;
}

int ent_ler_int(FILE *f, long min, long max, int *out) {
    char linha[TAM_LINHA];
    int rc = ler_linha(f, linha, sizeof linha);
    if (rc != ENT_OK) return rc;

    char *p = linha;
    while (isspace((unsigned char)*p)) p++;
    if (*p == '\0') return ENT_VAZIO;

    char *fim;
    errno = 0;
    long v = strtol(p, &fim, 10);
    if (fim == p) return ENT_INVALIDO;
    while (isspace((unsigned char)*fim)) fim++;
    if (*fim != '\0') return ENT_INVALIDO;
    if (errno == ERANGE || v > INT_MAX || v < INT_MIN) return ENT_OVERFLOW;
    if (v < min || v > max) return ENT_FORA_INTERVALO;

    *out = (int)v;
    return ENT_OK;
}

const char *ent_mensagem(int c) {
    switch (c) {
        case ENT_OK: return "ok";
        case ENT_EOF: return "fim da entrada";
        case ENT_VAZIO: return "campo vazio ou apenas espacos";
        case ENT_LONGO: return "texto acima de 79 caracteres";
        case ENT_INVALIDO: return "numero invalido";
        case ENT_OVERFLOW: return "numero grande demais (overflow)";
        case ENT_FORA_INTERVALO: return "valor fora do intervalo permitido";
        default: return "erro desconhecido";
    }
}
