#include "drone.h"

#include <stdlib.h>
#include "grafo.h"

struct _drone_ {
    unsigned altura_max;
    size_t x, y;

    Grafo mapa;
    size_t lin, col;
};

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
    Mapa novo = malloc(sizeof(struct _mapa_));
    novo->matriz = malloc(lin * sizeof(int *));
    for (size_t i = 0; i < lin; i++) {
        novo->matriz[i] = calloc(col, sizeof(int));
    }

    novo->lin = lin;
    novo->col = col;

    return novo;
}

void desaloca_mapa(Mapa mapa) {
    for (size_t i = 0; i < mapa->lin; i++) {
        free(mapa->matriz[i]);
    }
    free(mapa->matriz);

    free(mapa);
}

#define aresta(drone, linha, coluna) \
    (((linha) * ((drone)->col)) + (coluna))

#define linha(drone, chave) \
    ((chave) / ((drone)->lin))

#define coluna(drone, chave) \
    ((chave) % ((drone)->lin))

#ifdef DEBUG
#include <stdio.h>

void imprime_mapa(Mapa m) {
    for (size_t i = 0; i < m->lin; i++) {
        for (size_t j = 0; j < m->col; j++) {
            printf("%d ", m->matriz[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
#endif

void reconhece_mapa(Drone drone, Mapa mapa) {
    drone->mapa = novo_grafo(mapa->lin * mapa->col, 4);
    drone->lin = mapa->lin;
    drone->col = mapa->col;

#ifdef DEBUG
    printf("%lu %lu\n", drone->lin, drone->col);
    imprime_mapa(mapa);
#endif

    for (size_t i = 0; i < mapa->lin; i++) {
        for (size_t j = 0; j < mapa->col; j++) {
            if (mapa->matriz[i][j] <= drone->altura_max) {
                if (i+1 < mapa->lin && mapa->matriz[i+1][j] <= drone->altura_max) {
#ifdef DEBUG
                    printf("%lu %lu (%lu), %lu %lu (%lu) -> ", i, j, aresta(drone, i, j), i+1, j, 3 * j + (i+1));
#endif
                    insere_aresta(drone->mapa, aresta(drone, i, j), aresta(drone, i + 1, j));
                }
                if (j+1 < mapa->col && mapa->matriz[i][j+1] <= drone->altura_max) {
#ifdef DEBUG
                    printf("%lu %lu (%lu), %lu %lu (%lu) -> ", i, j, aresta(drone, i, j), i, j+1, 3 * (j+1) + i);
#endif
                    insere_aresta(drone->mapa, aresta(drone, i, j), aresta(drone, i, j + 1));
                }
#ifdef DEBUG
                    printf("\n");
#endif
            }
        }
    }

#ifdef DEBUG
    imprime_grafo(drone->mapa);
#endif
}

Mapa analisa_caminho(Drone drone, size_t xf, size_t yf) {
    Mapa mapa = malloc(sizeof(struct _mapa_));
    mapa->lin = drone->lin;
    mapa->col = drone->col;

    mapa->matriz = malloc(mapa->lin * sizeof(int *));
    for (size_t i = 0; i < mapa->lin; i++) {
        mapa->matriz[i] = malloc(mapa->col * sizeof(int));

        for (size_t j = 0; j < mapa->col; j++) {
            mapa->matriz[i][j] = -1;
        }
    }

    chave_t *caminho = percorre_em_profundidade(drone->mapa, aresta(drone, drone->x, drone->y), aresta(drone, xf, yf));

    for (size_t i = 0; caminho[i] != CHAVE_FINAL; i++) {
        mapa->matriz[linha(drone, caminho[i])][coluna(drone, caminho[i])] = i;
    }

    free(caminho);

    return mapa;
}