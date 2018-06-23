#include "grafo.h"

#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

struct _grafo_ {
    chave_t *adj_lst;
    size_t *adjacentes;
    size_t vertices, max_ar;
};

Grafo novo_grafo(size_t vertices, size_t max_arestas) {
    Grafo novo = malloc(sizeof(struct _grafo_));

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

#define adjlista(grafo, vertice, aresta) \
    grafo->adj_lst[((vertice) * ((grafo)->max_ar)) + (aresta)]

void insere_aresta(Grafo grafo, chave_t vert1, chave_t vert2) {
    size_t pos_ar = grafo->adjacentes[vert1]++;
    adjlista(grafo, vert1, pos_ar) = vert2;

    pos_ar = grafo->adjacentes[vert2]++;
    adjlista(grafo, vert2, pos_ar) = vert1;

#ifdef DEBUG
    printf("%lu %lu\n", vert1, vert2);
#endif
}

bool eh_adjacente(Grafo grafo, chave_t vert1, chave_t vert2) {
    for (size_t i = 0; i < grafo->max_ar; i++) {
        if (adjlista(grafo, vert1, i) == vert2) {
            return true;
        }
    }

    return false;
}

static chave_t *dfs_rec(Grafo grafo, bool *visitado, chave_t fim, chave_t pos, unsigned prof) {
    if (pos == fim) {
        chave_t *caminho = malloc((prof + 2) * sizeof(chave_t));
        caminho[prof + 1] = CHAVE_FINAL;
        caminho[prof] = pos;
        return caminho;
    }

    if (visitado[pos]) {
        return NULL;
    } else {
        visitado[pos] = true;
#ifdef DEBUG
        printf("%lu\n", pos);
#endif
    }

    for (size_t i = 0; i < grafo->adjacentes[pos]; i++) {
        chave_t *caminho = dfs_rec(grafo, visitado, fim, adjlista(grafo, pos, i), prof+1);
        if (caminho != NULL) {
            caminho[prof] = pos;
            return caminho;
        }
    }
    return NULL;
}

chave_t *percorre_em_profundidade(Grafo grafo, chave_t inicio, chave_t fim) {
    bool *visitados = calloc(grafo->vertices, sizeof(bool));
    chave_t *caminho = dfs_rec(grafo, visitados, fim, inicio, 0);
    free(visitados);
    return caminho;
}

#ifdef DEBUG
void imprime_grafo(Grafo g) {
    for (size_t i = 0; i < g->vertices; i++) {
        printf("%lu: ", i);

        for (size_t j = 0; j < g->adjacentes[i]; j++) {
            printf("%lu ", adjlista(g, i, j));
        }
        printf("\n");
    }
    printf("\n");
}
#endif