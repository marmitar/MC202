/*  Tiago de Paula Alves - 187679
 *  MC202 A - 2018s1
 */
#include "grafo.h"

/* * * * * * * * * * * *
 * BIBLIOTECA DE GRAFOS *
  * * * * * * * * * * * */

#include <stdlib.h>

struct _grafo_ {
    /* "lista" de adjacências */
    chave_t *adj_lst;
    /* vetor com quantidade de arestas em cada vértice */
    size_t *adjacentes;
    /* quantidade de vértices no grafo */
    size_t vertices;
    /* quantidade máxima de arestas em cada vértice */
    size_t max_ar;
};

/* CONSTRUTOR E DESTRUTOR */

Grafo novo_grafo(size_t vertices, size_t max_arestas) {
    Grafo novo = malloc(sizeof(struct _grafo_));

    /* matriz que representa a lista de adjacências */
    /* usa matriz aqui pois a memória máxima usada pode ser pevista do começo */
    novo->adj_lst = malloc(vertices * max_arestas * sizeof(chave_t));
    novo->vertices= 0;
    novo->vertices = vertices;

    novo->adjacentes = calloc(vertices, sizeof(size_t));
    novo->max_ar = max_arestas;

    return novo;
}
void destroi_grafo(Grafo grafo) {
    free(grafo->adj_lst);
    free(grafo->adjacentes);
    free(grafo);
}

/* INSERÇÃO */

/* acessa a matriz de adjacências */
#define adjlista(grafo, vertice, aresta) \
    grafo->adj_lst[((vertice) * ((grafo)->max_ar)) + (aresta)]

void insere_aresta(Grafo grafo, chave_t vert1, chave_t vert2) {
    size_t pos_aresta = grafo->adjacentes[vert1]++;
    adjlista(grafo, vert1, pos_aresta) = vert2;

    pos_aresta = grafo->adjacentes[vert2]++;
    adjlista(grafo, vert2, pos_aresta) = vert1;
}

/* TRANSVERSAL */

/* Transversal em profundidade recursiva, remontando o caminho percorrido */
static chave_t *dfs_rec(Grafo grafo, bool *visitado, chave_t fim, chave_t pos, unsigned prof) {
    size_t i;

    /* no final */
    if (pos == fim) {
        /* constroi o vetor do caminho */
        chave_t *caminho = malloc((prof + 1) * sizeof(chave_t));
        /* chave final representa o fim do caminho */
        caminho[prof] = CHAVE_FINAL;

        caminho[prof-1] = pos;
        return caminho;
    }

    if (visitado[pos]) {
        return NULL;
    } else {
        visitado[pos] = true;
    }

    /* para cada adjacente */
    for (i = 0; i < grafo->adjacentes[pos]; i++) {
        /* tenta achar o caminho */
        chave_t *caminho = dfs_rec(grafo, visitado, fim, adjlista(grafo, pos, i), prof+1);
        if (caminho != NULL) {
            caminho[prof-1] = pos;
            return caminho;
        }
    }
    return NULL;
}

chave_t *percorre_em_profundidade(Grafo grafo, chave_t inicio, chave_t fim) {
    /* vetor de vértices visitados, iniciado com falso */
    bool *visitados = calloc(grafo->vertices, sizeof(bool));

    chave_t *caminho = dfs_rec(grafo, visitados, fim, inicio, 1);

    free(visitados);
    return caminho;
}