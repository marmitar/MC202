#include <stdio.h>
#include "memoria.h"

#define MAX_OP_ARGS 3

int main(void) {
    /* número de operações e tamanho da memória */
    int N, S; scanf("%d %d", &N, &S);
    {
        Memoria mem = constroi_memoria(S);
        int i; /* índice do laço */
        for (i = 0; i < N; i++) {
            /* operação e argumentos da operação */
            char operacao; int args[MAX_OP_ARGS];
            
            scanf(" %c", &operacao);
            switch (operacao) {
                case 'A': /* operação de alocação */
                    scanf("%d", &(args[0]));

                    if (! aloca_memoria(mem, args[0])) {
                        fprintf(stderr, "LINHA %d: NÃO ALOCADO\n", i);
                    }
                break;
                case 'D': /* operação de desalocação */
                    scanf("%d %d", &(args[0]), &(args[1]));

                    if (desaloca_memoria(mem, args[0], args[1])) {
                        fprintf(stderr, "LINHA %d: DESALOCAÇÃO MÚLTIPLA\n", i);
                    }
                break;
                case 'R': /* operação de realocação */
                    scanf("%d %d %d", &(args[0]), &(args[1]), &(args[2]));

                    if (! realoca_memoria(mem, args[0], args[1], args[2])) {
                        fprintf(stderr, "LINHA %d: NÃO REALOCADO\n", i);
                    }
                break;
                case 'P': /* operação de impressão */
                    imprime_segmentos(mem);
                break;
            }
        }

        /* libera a memória da memória */
        destroi_memoria(mem);
    }

    return 0;
}