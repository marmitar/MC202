/* Tiago de Paula Alves - 187679
 * 2018s1 MC202 A - Lab. 12
 */
#ifndef GRAFO_H
#define GRAFO_H

/* lista de arestas adjacentes */
typedef struct Aresta Aresta;
typedef Aresta *Lista;
struct Aresta
{
    Aresta *prox;
    int v, d;
};

/* grafo com matriz de adjacências */
typedef struct Grafo Grafo;
struct Grafo
{
    int vertices;
    Lista *adjacencias;

    /* matriz com o pai de um vértice j em caminho partindo de i */
    int **caminhos;
    /* matriz com a distância de um vértice j partindo de i */
    int **distancias;
};

Grafo *criar_grafo(int vertices);
void destruir_grafo(Grafo *g);

/* aresta de peso d entre v1 e v2 */
void criar_aresta(Grafo *g, int v1, int v2, int d);

/* calcular menor distância entre v1 e v2, considerando um limite 'dist_max' para o peso da aresta */
int distancia(Grafo *g, int v1, int v2, int dist_max);
/* menor caminho */
int *caminho(Grafo *g, int v1, int v2, int dist_max);

/* controle da memória gerada */
void destruir_caminho(int *caminho);

#endif