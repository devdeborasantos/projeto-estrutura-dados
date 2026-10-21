#ifndef REGISTRO_H
#define REGISTRO_H

#define TAM_TEXTO 80   /* char[80]: 79 uteis + '\0' */
#define MAX_UTIL  79
#define ANO_MIN   1
#define ANO_MAX   2026

/* Tema C - Biblioteca: livros */
typedef struct {
    int  id;                 /* positivo e unico */
    char titulo[TAM_TEXTO];
    char autor[TAM_TEXTO];
    int  ano;                /* 1..2026 */
} Livro;

#endif
