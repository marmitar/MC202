#include <stdio.h>
#include "grafo.h"

#define VELOCIDADE 20
#define TEMPO_MAX 600
#define DIST_MAX VELOCIDADE * TEMPO_MAX

Grafo *ler_mapa() {
    int predios, caminhos;
    scanf("%d %d", &predios, &caminhos);

    Grafo *mapa = criar_grafo(predios);

    for (int i = 0; i < caminhos; i++) {
        int predio_1, predio_2, distancia;
        scanf("%d %d %d", &predio_1, &predio_2, &distancia);

        criar_aresta(mapa, predio_1, predio_2, distancia);
    }

    return mapa;
}

void imprime_percurso_inteiro(Grafo *m, int ini, int meio, int fim, int dist_max) {
    int *percurso = caminho(m, ini, meio, dist_max);
    int i;
    for (i = 0; percurso[i] >= 0; i++) {
        printf("%d ", percurso[i]);
    }
    destruir_caminho(percurso);

    percurso = caminho(m, meio, fim, dist_max);
    for (i = 1; percurso[i+1] >= 0; i++) {
        printf("%d ", percurso[i]);
    }
    printf("%d\n", percurso[i]);
    destruir_caminho(percurso);
}

void imprime_percurso_parcial(Grafo *m, int ini, int fim, int dist_max) {
    int *percurso = caminho(m, ini, fim, dist_max);
    int i;
    for (i = 0; percurso[i+1] >= 0; i++) {
        printf("%d ", percurso[i]);
    }
    printf("%d\n", percurso[i]);
    destruir_caminho(percurso);
}

int main(void) {

    Grafo *mapa = ler_mapa();

    int teia, aranha, bomba, mjane;
    scanf("%d %d %d %d", &teia, &aranha, &bomba, &mjane);

    int dist_ab = distancia(mapa, aranha, bomba, 2 * teia);
    int dist_am = distancia(mapa, aranha, mjane, 2 * teia);
    int dist_bm = distancia(mapa, bomba, mjane, 2 * teia);

    if (dist_ab + dist_bm <= DIST_MAX || dist_am + dist_bm <= DIST_MAX) {
        if (dist_ab <= dist_am) {
            imprime_percurso_inteiro(mapa, aranha, bomba, mjane, 2 * teia);
        } else {
            imprime_percurso_inteiro(mapa, aranha, mjane, bomba, 2 * teia);
        }
    } else {
        if (dist_ab <= dist_am) {
            imprime_percurso_parcial(mapa, aranha, bomba, 2 * teia);
        } else {
            imprime_percurso_parcial(mapa, aranha, mjane, 2 * teia);
        }
    }

    destruir_grafo(mapa);

    return 0;
}