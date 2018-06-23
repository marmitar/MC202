#ifndef __GRAFO_H__
#define __GRAFO_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

typedef struct _grafo_ *Grafo;
typedef uint64_t chave_t;
#define CHAVE_FINAL UINT64_MAX

Grafo novo_grafo(size_t vertices, size_t max_arestas_por_vertice);
void destroi_grafo(Grafo);

void insere_aresta(Grafo, chave_t, chave_t);

bool eh_adjacente(Grafo, chave_t, chave_t);

chave_t *percorre_em_profundidade(Grafo, chave_t inicio, chave_t fim);

#ifdef DEBUG
void imprime_grafo(Grafo g);
#endif

#endif/*__GRAFO_H__*/