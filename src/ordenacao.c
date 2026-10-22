#include "ordenacao.h"
#include <stdlib.h>
#include <string.h>

int comparar_livros(const Livro *a, const Livro *b) {
    int c = strcmp(a->titulo, b->titulo);
    if (c != 0) return c;
    return (a->id > b->id) - (a->id < b->id);
}

static void merge(Livro *v, Livro *tmp, int lo, int mid, int hi) {
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi)
        tmp[k++] = (comparar_livros(&v[i], &v[j]) <= 0) ? v[i++] : v[j++];
    while (i < mid) tmp[k++] = v[i++];
    while (j < hi)  tmp[k++] = v[j++];
    memcpy(v + lo, tmp + lo, (size_t)(hi - lo) * sizeof(Livro));
}

static void merge_sort(Livro *v, Livro *tmp, int lo, int hi) {
    if (hi - lo < 2) return;
    int mid = lo + (hi - lo) / 2;
    merge_sort(v, tmp, lo, mid);
    merge_sort(v, tmp, mid, hi);
    merge(v, tmp, lo, mid, hi);
}

/* Fallback sem memoria extra caso o malloc do buffer falhe. */
static void insertion_sort(Livro *v, int n) {
    for (int i = 1; i < n; i++) {
        Livro chave = v[i];
        int j = i - 1;
        while (j >= 0 && comparar_livros(&v[j], &chave) > 0) { v[j + 1] = v[j]; j--; }
        v[j + 1] = chave;
    }
}

void ordenar_livros(Livro *v, int n) {
    if (n < 2) return;
    Livro *tmp = malloc((size_t)n * sizeof(Livro));
    if (!tmp) { insertion_sort(v, n); return; }
    merge_sort(v, tmp, 0, n);
    free(tmp);
}
