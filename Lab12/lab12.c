/* Tiago de Paula Alves - 187679
 * 2018s1 MC202 A - Lab. 12
 */
#include <stdio.h>
#include "grafo.h"

#define VELOCIDADE    20.0
#define TEMPO_BOMBA  600.0
#define TEMPO_GAS    120.0
#define TEMPO_MAX   3600.0

Grafo *ler_mapa();
void imprime_percurso_inteiro(Grafo *m, int ini, int meio, int fim, int dist_max);
void imprime_percurso_parcial(Grafo *m, int ini, int fim, int dist_max);

#define tempo(dist) (((double) dist) / VELOCIDADE)

int main(void) {
    /*  Tempos dos possíveis percursos. Onde:
     * tempo_ab: tempo da posição inicial ('a'ranha)  até a 'b'omba;
     * tempo_amb: tempo da posição inicial até 'M'ary Jane e daí até a bomba;
     * ... e assim por diante.
     */
    double tempo_ab, tempo_am, tempo_abm, tempo_amb;

    Grafo *mapa = ler_mapa();

    int teia, aranha, bomba, mjane;
    scanf("%d %d %d %d", &teia, &aranha, &bomba, &mjane);

    tempo_ab = tempo(distancia(mapa, aranha, bomba, 2 * teia)) + TEMPO_BOMBA;
    tempo_am = tempo(distancia(mapa, aranha, mjane, 2 * teia)) + TEMPO_GAS;
    tempo_abm = tempo_ab + tempo(distancia(mapa, bomba, mjane, 2 * teia)) + TEMPO_GAS;
    tempo_amb = tempo_am + tempo(distancia(mapa, bomba, mjane, 2 * teia)) + TEMPO_BOMBA;

    /* logica da escolha do caminho */
    if (tempo_abm <= TEMPO_MAX || tempo_amb <= TEMPO_MAX) {
        if (tempo_abm <= tempo_amb) {
            imprime_percurso_inteiro(mapa, aranha, bomba, mjane, 2 * teia);
        } else {
            imprime_percurso_inteiro(mapa, aranha, mjane, bomba, 2 * teia);
        }
    } else {
        if (tempo_ab <= tempo_am) {
            imprime_percurso_parcial(mapa, aranha, bomba, 2 * teia);
        } else {
            imprime_percurso_parcial(mapa, aranha, mjane, 2 * teia);
        }
    }

    destruir_grafo(mapa);
    return 0;
}

Grafo *ler_mapa() {
    Grafo *mapa;
    int i, predios, caminhos;

    scanf("%d %d", &predios, &caminhos);

    mapa = criar_grafo(predios);

    for (i = 0; i < caminhos; i++) {
        int predio_1, predio_2, distancia;
        scanf("%d %d %d", &predio_1, &predio_2, &distancia);

        criar_aresta(mapa, predio_1, predio_2, distancia);
    }

    return mapa;
}

/* imprime apenas os termos centrais do caminho, ignorando os extremos */
void imprime_caminho(Grafo *g, int ini, int fim, int dist_max) {
    int i, *c;
    c = caminho(g, ini, fim, dist_max);

    for (i = 1; c[i+1] >= 0; i++) {
        printf("%d ", c[i]);
    }
    destruir_caminho(c);
}

void imprime_percurso_inteiro(Grafo *m, int ini, int meio, int fim, int dist_max) {
    printf("%d ", ini);
    imprime_caminho(m, ini, meio, dist_max);
    printf("%d ", meio);
    imprime_caminho(m, meio, fim, dist_max);
    printf("%d\n", fim);
}

void imprime_percurso_parcial(Grafo *m, int ini, int fim, int dist_max) {
    printf("%d ", ini);
    imprime_caminho(m, ini, fim, dist_max);
    printf("%d\n", fim);
}