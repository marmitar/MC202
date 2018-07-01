#include "grafo.h"

#include <stdlib.h>
#include "fila_prio.h"
#include <limits.h>

Grafo *criar_grafo(int vertices)
{
    Grafo *g = malloc(sizeof(Grafo));
    g->adjacencias = calloc(g->vertices, sizeof(Lista *));
    g->caminhos = calloc(g->vertices, sizeof(int *));
    g->distancias = calloc(g->vertices, sizeof(int *));
    g->vertices = vertices;
    return g;
}

void destruir_lista(Lista *l)
{
    while (*l != NULL) {
        Aresta *no = *l;
        (*l) = (*l)->prox;
        free(no);
    }
}

void destruir_grafo(Grafo *g)
{
    if (g != NULL) {
        for (int i = 0; i < g->vertices; i++) {
            destruir_lista(&(g->adjacencias[i]));
            if (g->caminhos[i] != NULL) {
                free(g->distancias[i]);
                free(g->caminhos[i]);
            }
        }
        free(g->adjacencias);
        free(g->distancias);
        free(g->caminhos);
        free(g);
    }
}

void inserir_adjacencia(Lista *l, int v, int d)
{
    Aresta *a = malloc(sizeof(Aresta));
    a->prox = *l;
    a->v = v;
    a->d = d;
    *l = a;
}

void criar_aresta(Grafo *g, int v1, int v2, int d)
{
    inserir_adjacencia(&(g->adjacencias[v1]), v2, d);
    inserir_adjacencia(&(g->adjacencias[v2]), v1, d);
}

void relaxa_caminho(Grafo *g, FilaPrio *fp, int *pai, int *dists, int v, Aresta *u)
{
    if (dists[v] + u->d < dists[u->v]) {
        dists[u->v] = dists[v] + u->d;
        pai[u->v] = v;

        diminuir_prioridade(fp, u->v, dists[u->v]);
    }
}

void dijkstra(Grafo *g, int s, int dist_max)
{
    g->caminhos[s] = malloc(g->vertices * sizeof(int));
    g->distancias[s] = malloc(g->vertices * sizeof(int));
    FilaPrio *fila = criar_fila_prio(g->vertices);

    for (int v = 0; v < g->vertices; v++) {
        g->distancias[s][v] = -1;
        inserir(fila, v, INT_MAX);
        g->distancias[s][v] = INT_MAX;
    }

    diminuir_prioridade(fila, s, 0);
    g->distancias[s][s] = 0;

    while (! eh_vazia(fila)) {
        int v = extrair_minimo(fila);

        if (g->distancias[s][v] != INT_MAX) {
            for (Aresta *t = g->adjacencias[v]; t != NULL; t = t->prox) {
                if (t->d <= dist_max) {
                    relaxa_caminho(g, fila, g->caminhos[s], g->distancias[s], v, t);
                }
            }
        }
    }

    destruir_fila_prio(fila);
}

int distancia(Grafo *g, int ini, int fim, int dist_max)
{
    if (g->caminhos[ini] == NULL) {
        dijkstra(g, ini, dist_max);
    }

    return g->distancias[ini][fim];
}

int *caminho(Grafo *g, int ini, int fim, int dist_max)
{
    if (g->caminhos[ini] == NULL) {
        dijkstra(g, ini, dist_max);
    }

    int vertices = 0;
    int *c = malloc(g->vertices*sizeof(int));
    for (int v = fim; v >= 0; v = g->caminhos[ini][v]) {
        c[vertices] = v;
        vertices++;
    }

    int *c_red = malloc((vertices+1)*sizeof(int));
    for (int i = 0; i < vertices; i++) {
        c_red[i] = c[vertices-i-1];
    }
    c_red[vertices] = -1;

    free(c);
    return c_red;
}

void destruir_caminho(int *caminho)
{
    free(caminho);
}