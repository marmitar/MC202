#ifndef __GRAFO_H__
#define __GRAFO_H__

typedef struct Aresta Aresta;
typedef Aresta *Lista;
struct Aresta
{
    Aresta *prox;
    int v, d;
};

typedef struct Grafo Grafo;
struct Grafo
{
    int vertices;
    Lista *adjacencias;
    int **caminhos;
    int **distancias;
};

Grafo *criar_grafo(int vertices);
void destruir_grafo(Grafo *g);

void criar_aresta(Grafo *g, int v1, int v2, int d);

int eh_vertice(Grafo *g, int v);

int distancia(Grafo *g, int ini, int fim, int dist_max);
int *caminho(Grafo *g, int ini, int fim, int dist_max);
void destruir_caminho(int *caminho);

#endif/*__GRAFO_H__*/