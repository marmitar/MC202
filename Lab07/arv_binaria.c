#include "arv_binaria.h"

#include <stdlib.h>
#include <stdbool.h>

typedef struct _no {
    Item dado;
    struct _no *pai;
    struct _no *dir, *esq;
} *No;

struct _arv_b {
    No raiz;
    size_t tamanho;
};

#define eh_vazio(ptr) \
    (ptr == NULL)

#define novo_no(noh, dado, pai, dir, esq) \
    noh = malloc(sizeof(struct _no)); \
    noh->dado = dado; \
    noh->pai = pai; \
    noh->dir = dir; \
    noh->esq = esq

Arvore constroi_arvore(Comparador comparador, Destrutor destrutor);
unsigned destroi_arvore(Arvore arvore);

bool inserir_item(Arvore arvore, Item item);
Item pegar_proximo(Arvore arvore);