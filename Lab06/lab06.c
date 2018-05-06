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
    FilaPrio *dependentes = malloc(N * sizeof(FilaPrio));
    for (int n = 0; n < N; n++) {
        dependentes[n] = constroi_filaprio(P / 8, insere_proc, compara_prio, destroi_proc);
    }
    int *tempos = calloc(N, sizeof(int));
    
    bool acabou = false;
    for (Processo proximo = pegar_proximo(em_espera); ! acabou;) {
        bool fim = false;
        int min = 0;
        for (int n = 0; !fim && n < N; n++) {
            if (processos[n] == 0) {
                if (esta_vazia(dependentes[n])) {
                    processos[n] = proximo->id;
                    tempos[n] = proximo->clocks;
                    inicia_proc(processos[n], clocks);
                    destroi_proc(proximo);
                    fim = true;
                } else {
                    Processo dependente = pegar_proximo(dependentes[n]);
                    processos[n] = dependente->id;
                    tempos[n] = dependente->clocks;
                    inicia_proc(processos[n], clocks);
                    destroi_proc(dependente);
                }
            } else if (processos[n] == proximo->dependencia) {
                novo_item(dependentes[n], proximo);
                fim = true;
            } else if (tempos[n] < tempos[min]) {
                min = n;
            }
        }

        if (! fim) {
            clocks += tempos[min];
            for (int n = 0; n < N; n++) {
                tempos[n] -= tempos[min];
                if (tempos[n] == 0) {
                    encerra_proc(processos[n], clocks);
                    processos[n] = 0;
                }
            }
        } else {
            proximo = pegar_proximo(em_espera);
        }
    }

    return 0;
}