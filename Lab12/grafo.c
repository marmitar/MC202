/* Tiago de Paula Alves - 187679
 * 2018s1 MC202 A - Lab. 12
 */
#include "grafo.h"

#include <stdlib.h>
#include "fila_prio.h"
#include <limits.h>

/* * * * * * * * * * * * *
 * Construtor e destrutor *
  * * * * * * * * * * * * */

Grafo *criar_grafo(int vertices)
{
    Grafo *g = malloc(sizeof(Grafo));
    g->adjacencias = calloc(vertices, sizeof(Lista *));
    g->caminhos = calloc(vertices, sizeof(int *));
    g->distancias = calloc(vertices, sizeof(int *));
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
    int i; for (i = 0; i < g->vertices; i++) {
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


/* * * * * * * * * * * * *
 * Ligação entre vértices *
  * * * * * * * * * * * * */

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


/* * * * * * * *
 * Transversal *
 * * * * * * * */

/* reduz distância parcial (de 's') do vértice 'v', passando por 'u', se possível */
void relaxa_caminho(Grafo *g, FilaPrio *fp, int *pai, int *dists, int v, Aresta *u)
{
    if (dists[v] + u->d < dists[u->v]) {
        dists[u->v] = dists[v] + u->d;
        pai[u->v] = v;

        diminuir_prioridade(fp, u->v, dists[u->v]);
    }
}

/* Dijkstra com limite de peso de aresta. */
void dijkstra(Grafo *g, int s, int dist_max)
{
    int v;

    FilaPrio *fila = criar_fila_prio(g->vertices);
    /* inicializa os vetores de caminhos e distancias do vértice s */
    g->caminhos[s] = malloc(g->vertices * sizeof(int));
    g->distancias[s] = malloc(g->vertices * sizeof(int));

    for (v = 0; v < g->vertices; v++) {
        g->caminhos[s][v] = -1;
        g->distancias[s][v] = INT_MAX;
        inserir(fila, v, g->distancias[s][v]);
    }

    g->distancias[s][s] = 0;
    diminuir_prioridade(fila, s, g->distancias[s][s]);

    while (! eh_vazia(fila)) {
        v = extrair_minimo(fila);

        if (g->distancias[s][v] != INT_MAX) {
            Aresta *t;
            for (t = g->adjacencias[v]; t != NULL; t = t->prox) {

                if (t->d <= dist_max) { /* limite de peso da aresta */
                    relaxa_caminho(g, fila, g->caminhos[s], g->distancias[s], v, t);
                }
            }
        }
    }

    destruir_fila_prio(fila);
}


/* * * * * *
 * Percurso *
  * * * * * */

int distancia(Grafo *g, int v1, int v2, int dist_max)
{
    /* calcula todas as distâncias partindo de v1, se ainda não foi feito */
    if (g->caminhos[v1] == NULL) {
        dijkstra(g, v1, dist_max);
    }

    return g->distancias[v1][v2];
}

int *caminho(Grafo *g, int v1, int v2, int dist_max)
{
    int vertices, v, *caminho_maior, *caminho_reduzido;

    /* calcula todas os caminhos partindo de v1, se ainda não foi feito */
    if (g->caminhos[v1] == NULL) {
        dijkstra(g, v1, dist_max);
    }

    /* quantidade de vértices no caminho */
    vertices = 0;
    caminho_maior = malloc(g->vertices*sizeof(int));
    /* procura o caminho */
    for (v = v2; v >= 0; v = g->caminhos[v1][v]) {
        caminho_maior[vertices] = v;
        vertices++;
    }

    /* redução da memória utilizada para manter o caminho */
    /* e inversão da ordem */
    caminho_reduzido = malloc((vertices+1)*sizeof(int));
    for (v = 0; v < vertices; v++) {
        caminho_reduzido[v] = caminho_maior[vertices-v-1];
    }
    /* marca o final do caminho */
    caminho_reduzido[vertices] = -1;

    free(caminho_maior);
    return caminho_reduzido;
}

void destruir_caminho(int *caminho)
{
    free(caminho);
}