#ifndef _ARV_BIN_H
#define _ARV_BIN_H

#include <stdbool.h>

typedef void *Item;

typedef struct _arv_b *Arvore;

typedef int (*Comparador)(Item, Item);
typedef void (*Destrutor)(Item);

Arvore constroi_arvore(Destrutor, Comparador);
unsigned destroi_arvore(Arvore);

bool inserir_item(Arvore, Item);
int buscar_item(Arvore, Item);

#endif