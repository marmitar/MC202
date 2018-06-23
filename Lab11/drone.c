/*  Tiago de Paula Alves - 187679
 *  MC202 A - 2018s1
 */
#include "drone.h"

/* * * * * * * * * * * * * * * * *
 * BIBLIOTECA DO SISTEMA DO DRONE *
  * * * * * * * * * * * * * * * * */

#include <stdlib.h>
#include "grafo.h"

struct _drone_ {
    /* dados do drone em si */
    unsigned altura_max;
    size_t x, y;

    /* dados do mapa */
    Grafo mapa;
    size_t lin, col;
};

/* CONSTRUTORES E DESTRUTORES */

Drone novo_drone(unsigned altura_max, size_t x0, size_t y0) {
    Drone novo = calloc(1, sizeof(struct _drone_));
    novo->altura_max = altura_max;
    novo->x = x0;
    novo->y = y0;

    return novo;
}
void destroi_drone(Drone drone) {
    destroi_grafo(drone->mapa);
    free(drone);
}


Mapa aloca_mapa(size_t lin, size_t col) {
    size_t i;
    Mapa novo = malloc(sizeof(struct _mapa_));

    novo->matriz = malloc(lin * sizeof(int *));
    for (i = 0; i < lin; i++) {
        novo->matriz[i] = calloc(col, sizeof(int));
    }

    novo->lin = lin;
    novo->col = col;

    return novo;
}
void desaloca_mapa(Mapa mapa) {
    size_t i; for (i = 0; i < mapa->lin; i++) {
        free(mapa->matriz[i]);
    }
    free(mapa->matriz);

    free(mapa);
}

/* RELAÇÃO CHAVE x POSIÇÃO */

/* calcula a chave com base na posição */
#define vertice(drone, linha, coluna) \
    (((linha) * ((drone)->col)) + (coluna))

/* calcula a posição a partir da chave */
#define linha(drone, chave) \
    ((chave) / ((drone)->col))
#define coluna(drone, chave) \
    ((chave) % ((drone)->col))

/* monta o grafo da região com base na matriz do mapa */
void reconhece_mapa(Drone drone, Mapa mapa) {
    long i;

    /* são (linhas x colunas) vértices e no máximo 4 adjacentes */
    drone->mapa = novo_grafo(mapa->lin * mapa->col, 4);
    drone->lin = mapa->lin;
    drone->col = mapa->col;

    /* monta o grafo pelo canto inferior direito */
    for (i = mapa->lin-1; i >= 0; i--) {
        long j; for (j = mapa->col-1; j >= 0; j--) {

            if (mapa->matriz[i][j] <= drone->altura_max) {
                /* insere aresta se ambos os vértices forem acessíveis */
                if (i-1 >= 0 && mapa->matriz[i-1][j] <= drone->altura_max) {
                    insere_aresta(drone->mapa, vertice(drone, i, j), vertice(drone, i - 1, j));
                }
                if (j-1 >= 0 && mapa->matriz[i][j-1] <= drone->altura_max) {
                    insere_aresta(drone->mapa, vertice(drone, i, j), vertice(drone, i, j - 1));
                }
            }
        }
    }
}

/* encontra um caminho até a base em xf, yf */
Mapa analisa_caminho(Drone drone, size_t xf, size_t yf) {
    size_t i; chave_t *caminho;

    /* mapa do caminho */
    Mapa mapa = malloc(sizeof(struct _mapa_));
    mapa->lin = drone->lin;
    mapa->col = drone->col;

    /* considera que não acessou nenhuma posição no começo */
    mapa->matriz = malloc(mapa->lin * sizeof(int *));
    for (i = 0; i < mapa->lin; i++) {
        size_t j;
        mapa->matriz[i] = malloc(mapa->col * sizeof(int));

        for (j = 0; j < mapa->col; j++) {
            mapa->matriz[i][j] = -1;
        }
    }

    /* acha caminho */
    caminho = percorre_em_profundidade(drone->mapa, vertice(drone, drone->y, drone->x), vertice(drone, yf, xf));
    for (i = 0; caminho[i] != CHAVE_FINAL; i++) {
        mapa->matriz[linha(drone, caminho[i])][coluna(drone, caminho[i])] = i;
    }

    free(caminho);
    return mapa;
}