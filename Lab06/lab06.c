#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

typedef struct _proc {
    int id;
    int clocks;
    int prioridade;
    int dependencia;
    int iniciou;
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

int compara_exec(Processo proc1, Processo proc2) {
    if (proc1->dependencia == proc2->id) {
        return -1;
    }
    if (proc2->dependencia == proc1->id) {
        return 1;
    }
    return (proc2->iniciou + proc2->clocks) - (proc1->iniciou + proc1->clocks);
}

Processo insere_proc(va_list argumentos) {
    return va_arg(argumentos, Processo);
}

#define inicia_proc(proc, clock) \
    printf("processo %d iniciou no clock %d\n", proc->id, clock);

#define encerra_proc(proc, clock) \
    printf("processo %d encerrou no clock %d\n", proc->id, clock);

#define tempo(processo, clock) \
    (processo->clocks - (clock - processo->iniciou))

#define de_cada_n(i, n) \
    for (i = 0; i < n; i++)

int main(void) {
    int N, P;
    scanf("%d %d", &N, &P);
    FilaPrio agendador = constroi_filaprio(P, insere_proc, compara_prio, destroi_proc);

    for (int i = 0; i < P; i++) {
        int I, E, H, D;
        scanf("%d %d %d %d", &I, &E, &H, &D);
        novo_item(agendador, I, E, H, D);
    }

    int clocks = 0;
    int *processos = calloc(N, sizeof(int));
    int *tempos = calloc(N, sizeof(int));

    for (int p = 0; p < P; p++) {
        Processo proximo = pegar_proximo(agendador);
        bool dependente = processos[0] == proximo->dependencia;
        int min = 0;

        for (int n = 1; n < N; n++) {
            if (tempos[n] < tempos[min]) {
                min = n;
            }
            if (processos[n] == proximo->dependencia) {
                dependente = true;
            }
        }

        if (dependente) {
            // dependencia
        } else if (processos[min] == 0) {
            processos[min] = proximo->id;
            tempos[min] = proximo->clocks;
            // inicia
            destroi_proc(proximo);
        } else {
            clocks += tempos[min];
            for (int n = 0; n < N; n++) {
                tempos[n] -= tempos[min];

                if (tempos[n] == 0) {
                    // encerra
                    processos[n] = 0;
                }
            }

            processos[min] = proximo->id;
            tempos[min] = proximo->clocks;
            // inicia
            destroi_proc(proximo);
        }
        // senão
            // encerra o proximo
            // se estiver dependente
                // acha o proximo que pode
            // executa
    }

    return 0;
}