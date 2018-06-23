#include <stdio.h>

#include "drone.h"

Mapa le_mapa(int lins, int cols) {
    Mapa mapa = aloca_mapa(lins, cols);

    for (int i = 0; i < lins; i++) {
        for (int j = 0; j < cols; j++) {
            scanf(" %d", &(mapa->matriz[i][j]));
        }
    }

    return mapa;
}

void escreve_caminho(Mapa caminho) {
    for (int i = 0; i < caminho->lin; i++) {
        for (int j = 0; j < caminho->col; j++) {
            if (caminho->matriz[i][j] >= 0) {
                printf("%d ", caminho->matriz[i][j]);
            } else {
                printf("# ");
            }
        }
        printf("\n");
    }
}

int main(void) {
    int H, X, Y;
    int A, B, M, N;

    scanf("%d %d %d", &H, &X, &Y);
    scanf("%d %d %d %d", &A, &B, &M, &N);

    Drone drone = novo_drone(H, X, Y);

    Mapa mapa = le_mapa(M, N);
    reconhece_mapa(drone, mapa);
    desaloca_mapa(mapa);

    mapa = analisa_caminho(drone, A, B);
    escreve_caminho(mapa);
    desaloca_mapa(mapa);

    destroi_drone(drone);
    return 0;
}