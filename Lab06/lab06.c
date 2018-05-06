#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

typedef struct _proc {
    int id;
    int clocks;
    int prioridade;
    int dependencia;
} *Processo;
#define TIPO Processo
#include "fila_prioridade.h"

Processo constroi_proc(va_list argumentos) {
    Processo novo = (Processo) malloc(sizeof(struct _proc));
    novo->id = va_arg(argumentos, int);
    novo->clocks = va_arg(argumentos, int);
    novo->prioridade = va_arg(argumentos, int);
    novo->dependencia = va_arg(argumentos, int);
    return novo;
}

Destrutor destroi_proc = (Destrutor) free;

int compara_prio(Processo proc1, Processo proc2) {
    if (proc1->dependencia == proc2->id) {
        return -1;
    }
    if (proc2->dependencia == proc1->id) {
        return 1;
    }
    if (proc1->prioridade == proc2->prioridade) {
        return proc1->id - proc2->id;
    }

    return proc1->prioridade - proc2->prioridade;
}

Processo insere_proc(va_list argumentos) {
    return va_arg(argumentos, Processo);
}

#define inicia_proc(proc, clock) \
    printf("processo %d iniciou no clock %d\n", proc, clock);

#define encerra_proc(proc, clock) \
    printf("processo %d encerrou no clock %d\n", proc, clock);

int main(void) {
    int N, P;
    scanf("%d %d", &N, &P);

    FilaPrio em_espera = constroi_filaprio(P, constroi_proc, compara_prio, destroi_proc);

    for (int i = 0; i < P; i++) {
        int I, E, H, D;
        scanf("%d %d %d %d", &I, &E, &H, &D);
        novo_item(em_espera, I, E, H, D);
    }

    int clocks = 0;
    int *processos = calloc(N, sizeof(int));
    // FilaPrio *dependentes = malloc(N * sizeof(FilaPrio));
    // for (int n = 0; n < N; n++) {
    //     dependentes[n] = constroi_filaprio(P / 8, insere_proc, compara_prio, destroi_proc);
    // }
    Processo *dependentes = calloc(N, sizeof(Processo));
    int tamanho_dep = 0;
    int *tempos = calloc(N, sizeof(int));

    for (Processo proximo = pegar_proximo(em_espera); proximo != NULL;) {
        bool depende = false;
        int min = 0;
        for (int n = N-1; !depende && n >= 0; n--) {
            if (processos[n] != 0) {
                if (processos[n] == proximo->dependencia) {
                    dependentes[tamanho_dep] = proximo;
                    tamanho_dep++;
                    depende = true;
                } else if (tempos[n] <= tempos[min]) {
                    min = n;
                }
            } else {
                min = n;
            }
        }

        if (! depende) {
            clocks += tempos[min];
            for (int n = 0; n < N; n++) {
                tempos[n] -= tempos[min];
                if (tempos[n] == 0) {
                    encerra_proc(processos[n], clocks);
                    processos[n] = 0;
                }
            }

            processos[min] = proximo->id;
            tempos[min] = proximo->clocks;

            while (tamanho_dep > 0) {
                tamanho_dep--;
                inserir_item(em_espera, dependentes[tamanho_dep]);
            }
        }

        proximo = pegar_proximo(em_espera);
    }

    return 0;
}