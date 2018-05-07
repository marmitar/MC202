#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* tipo processo, para a fila de prioridades */
typedef struct _proc {
    int id;
    int clocks;
    int prioridade;
} *Processo;
#define TIPO Processo
#include "fila_prioridade.h"

/* construtor */
Processo novo_proc(int id, int clocks, int prioridade) {
    Processo novo = (Processo) malloc(sizeof(struct _proc));
    novo->id = id;
    novo->clocks = clocks;
    novo->prioridade = prioridade;
    return novo;
}
/* destrutor (é só um 'free' na verdade) */
Destrutor destroi_proc = (Destrutor) free;

/* comparação de prioridades */
int compara_prio(Processo proc1, Processo proc2) {
    if (proc1->prioridade == proc2->prioridade) {
        return proc2->id - proc1->id;
    }

    return proc1->prioridade - proc2->prioridade;
}

/* estrutura de um "processador" */
typedef struct _cpu {
    int *processos; /* vetor de processos em execução */
    int *tempos; /* vetor de tempos restantes de cada processo */
    int tamanho; /* quantidade de núcleos */
    int clocks; /* tempo em funcionamento */
    int processados; /* processos executados */
} *Processador;

/* construtor e destrutor */
Processador constroi_cpu(int tamanho) {
    Processador novo = malloc(sizeof(struct _cpu));
    novo->tamanho = tamanho;
    novo->processos = calloc(tamanho, sizeof(int));
    novo->tempos = calloc(tamanho, sizeof(int));
    novo->clocks = 0;
    novo->processados = 0;
    return novo;
}
void destroi_cpu(Processador cpu) {
    free(cpu->processos);
    free(cpu->tempos);
    free(cpu);
}

/* coloca processo no processador */
void inicia_processo(Processador nucleos, int pos, Processo proc) {
    nucleos->processos[pos] = proc->id;
    nucleos->tempos[pos] = proc->clocks;
    printf("processo %d iniciou no clock %d\n", proc->id, nucleos->clocks);
    destroi_proc(proc);
}

/* retorna o npúcleo que está mais próximo de terminar */
/* pode considerar núcleos vazios ou não */
int proximo_a_terminar(Processador nucleos, bool considerar_vazios) {
    int n, min;

    min = 0;
    for (n = nucleos->tamanho-1; n >= 0; n--) {
        /* núcleo vazio */
        if (considerar_vazios && nucleos->processos[n] == 0) {
            min = n;
            nucleos->tempos[min] = 0;

        } else if ((considerar_vazios && nucleos->tempos[n] <= nucleos->tempos[min])
        /* se não puder os considerar vazios, tem que analisar se o núcleo atual ou o menor até então são vazios */
                || (! considerar_vazios && (nucleos->processos[min] == 0
                    || (nucleos->processos[n] != 0 && nucleos->tempos[n] <= nucleos->tempos[min])))) { 
            min = n;
        }
    }
    return min;
}

/* Faz o processador avançar um tempo avisando as execuções encerradas nesse tempo.
 * Se um processo encerrado tiver processos dependentes, insere esses processos e
 * retorna 'true' avisando a inserção.
 */
bool avanca_clocks(Processador nucleos, FilaPrio processos, int clocks, Processo **dependentes, int *tam_deps) {
    int n;

    bool inseriu_processo = false;
    nucleos->clocks += clocks;

    for (n = 0; n < nucleos->tamanho; n++) {
        nucleos->tempos[n] -= clocks;

        /* acabou o tempo de execução do processo */
        if (nucleos->processos[n] != 0 && nucleos->tempos[n] <= 0) {
            printf("processo %d encerrou no clock %d\n", nucleos->processos[n], nucleos->clocks + nucleos->tempos[n]);

            nucleos->processados++;

            /* dependencias */
            while (tam_deps[nucleos->processos[n]-1] > 0) {
                inserir_item(processos, dependentes[nucleos->processos[n]-1][--tam_deps[nucleos->processos[n]-1]]);
                inseriu_processo = true;
            }

            nucleos->processos[n] = 0; /* vazio */
        }
    }

    return inseriu_processo;
}

int main(void) {
    FilaPrio em_espera;
    Processador processos;
    Processo **dependentes;
    int *tamanho_deps;
    Processo proximo;
    int i;

    int N, P;
    scanf("%d %d", &N, &P);

    /* escalonador de processos */
    em_espera = constroi_filaprio(P, compara_prio, destroi_proc);
    /* processos dependentes */
    dependentes = malloc(P * sizeof(Processo *));
    tamanho_deps = calloc(P, sizeof(int));
    for (i = 0; i < P; i++) {
        dependentes[i] = malloc(P * sizeof(Processo));
    }

    /* leitura dos processos */
    for (i = 0; i < P; i++) {
        int I, E, H, D;
        scanf("%d %d %d %d", &I, &E, &H, &D);
        if (D == 0) {
            inserir_item(em_espera, novo_proc(I, E, H));
        } else {
            dependentes[D-1][tamanho_deps[D-1]++] = novo_proc(I, E, H);
        }
    }
    
    /* processador de N núcleos */
    processos = constroi_cpu(N);

    /* até processar todos os processos */
    for (proximo = pegar_proximo(em_espera); processos->processados < P; proximo = pegar_proximo(em_espera)) {

        int min = proximo_a_terminar(processos, proximo != NULL);

        bool mudou_escalonador = avanca_clocks(processos, em_espera, processos->tempos[min], dependentes, tamanho_deps);

        if (proximo != NULL) {
            if (mudou_escalonador) {
                /* volta para o escalonador */
                inserir_item(em_espera, proximo);
            } else {
                /* inicia próximo no núcleo 'min' */
                inicia_processo(processos, min, proximo);
            }
        }
    }

    /* limpa memória */
    for (i = 0; i < P; i++) {
        free(dependentes[i]);
    }
    free(dependentes);
    free(tamanho_deps);
    destroi_cpu(processos);
    destroi_filaprio(em_espera);
    return 0;
}