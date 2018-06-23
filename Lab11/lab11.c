/*  Tiago de Paula Alves - 187679
 *  MC202 A - 2018s1
 */
#include <stdio.h>

#include "drone.h"

Mapa le_mapa(int lins, int cols) {
    Mapa mapa = aloca_mapa(lins, cols);

    int i; for (i = 0; i < lins; i++) {
        int j; for (j = 0; j < cols; j++) {
            scanf(" %d", &(mapa->matriz[i][j]));
        }
    }

    return mapa;
}

void escreve_caminho(Mapa caminho) {
    int i; for (i = 0; i < caminho->lin; i++) {
        int j; for (j = 0; j < caminho->col; j++) {
            if (caminho->matriz[i][j] >= 0) {
                printf("%d ", caminho->matriz[i][j]);
            } else {
                /* caminho não percorrido */
                printf("# ");
            }
        }
        printf("\n");
    }
}

int main(void) {
    int H, X, Y;
    int A, B, M, N;
    Drone drone;
    Mapa mapa;

    scanf("%d %d %d", &H, &X, &Y);
    scanf("%d %d %d %d", &A, &B, &M, &N);

    drone = novo_drone(H, X, Y);

    mapa = le_mapa(M, N);
    reconhece_mapa(drone, mapa);
    desaloca_mapa(mapa);

    /* caminho */
    mapa = analisa_caminho(drone, A, B);
    escreve_caminho(mapa);
    desaloca_mapa(mapa);

    destroi_drone(drone);
    return 0;
}