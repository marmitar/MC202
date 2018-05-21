#ifndef _ARV_BIN_H
#define _ARV_BIN_H

#include <stdlib.h>
#include <stdbool.h>

typedef void *Item;

typedef enum {PREORDEM, POSORDEM, INORDEM} Ordem;

typedef struct _no {
    Item dado;
    struct _no *direita;
    struct _no *esquerda;
} *No;

typedef int (*Comparador)(Item, Item);
typedef struct _arv_b {
    Comparador comparador;
    No raiz;
} *Arvore;

Arvore constroi_arvore(Item *lista, size_t tamanho, Ordem, Comparador);
unsigned destroi_arvore(Arvore);

int buscar_item(Arvore, Item);

#endif