#ifndef __GRAFO_H__
#define __GRAFO_H__

#include <stddef.h>

typedef void *Vértice;
typedef struct _grafo_ *Grafo;

Grafo novo_grafo(size_t tamanho);
void destroi_grafo(Grafo);

#endif/*__GRAFO_H__*/