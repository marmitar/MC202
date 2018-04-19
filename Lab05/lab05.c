#include <stdio.h>
#include <stdbool.h>
#include "pilha.h"

bool tem_desfile(const int *carros, const int qtd_carros, const int qtd_ruas) {
    int *ruas = calloc(qtd_ruas, sizeof(int));
    int ultimo_carro = 0;
    
    for (int i = 0; i < qtd_carros; i++) {
        if (carros[i] == ultimo_carro + 1) {
            ultimo_carro++;
        } else {
            bool encaixou = false;
            for (int j = 0; j < qtd_ruas && !encaixou; j++) {
                if (ruas[j] > carros[i]) {
                    ruas[j] = carros[i];
                    encaixou = true;
                }
            }
            if (!encaixou) {
                free(ruas);
                return false;
            }
        }
    }

    free(ruas);
    return true;
}

int main(void) {
    
    int n, r; scanf("%d %d", &n, &r);

    int *carros = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        scanf("%d", &(carros[i]));
    }

    if (! tem_desfile(carros, n, r)) {
        printf("Sem desfile.\n");
        return 1;
    }

    printf("Desfile pode ser realizado:\n");

    Pilha *ruas = malloc(r * sizeof(Pilha));
    for (int i = 0; i < r; i++) {
        ruas[i] = constroi_pilha();
    }

    int ultimo = 0;
    for (int i = 0; i < n; i++) {
        if (carros[i] == ultimo+1) {
            ultimo++;
            printf("Alegoria %d desfila.", carros[i]);
        } else {
            bool encaixou = false;
            for (int j = 0; j < r && !encaixou; j++) {
                int *topo = pegar_proximo(ruas[j]);
                if (topo == NULL || *topo > carros[i]) {
                    empilhar(ruas[j], &(carros[i]));
                    printf("Alegoria %d entra na rua %d e aguarda.", carros[i], j+1);
                }
            }
            if (!encaixou) {
                free(ruas);
                return false;
            }
        }
    }

    return 0;
}