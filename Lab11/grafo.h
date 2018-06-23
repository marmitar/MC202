/*  Tiago de Paula Alves - 187679
 *  MC202 A - 2018s1
 */
#ifndef __GRAFO_H__
#define __GRAFO_H__

/* * * * * * * * * * * *
 * BIBLIOTECA DE GRAFOS *
  * * * * * * * * * * * */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

typedef struct _grafo_ *Grafo;
/* chave usada nos grafos */
typedef uint64_t chave_t;
/* chave reservada */
#define CHAVE_FINAL UINT64_MAX

/* Construtor e destrutor */
Grafo novo_grafo(size_t vertices, size_t max_arestas_por_vertice);
void destroi_grafo(Grafo);

/* insere aresta entre duas chaves */
void insere_aresta(Grafo, chave_t, chave_t);

/* monta um vetor de chaves para representar o caminho usando DFS */
/* vetor encerra em CHAVE_FINAL */
chave_t *percorre_em_profundidade(Grafo, chave_t inicio, chave_t fim);

#endif/*__GRAFO_H__*/