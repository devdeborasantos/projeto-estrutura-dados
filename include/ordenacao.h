#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "registro.h"

/* Comparacao por titulo (strcmp, sensivel a caixa); empate desempatado por ID crescente. */
int  comparar_livros(const Livro *a, const Livro *b);

/* Merge Sort proprio (estavel, O(n log n)) sobre vetor de copias. Nao usa qsort(). */
void ordenar_livros(Livro *v, int n);

#endif
