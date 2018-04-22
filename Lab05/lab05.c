/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 05
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include "pilha.h"

/* Operações no desfile. */
typedef enum {DESFILA, ENTRA, SAI} Operacao;

/* Registro de movimento.
 *  Pode ser um desfile de uma alegoria, a entrada ou a saída de uma alegoria de uma perpendicular.
 */
typedef struct _mov {
    Operacao operacao;
    unsigned alegoria;
    unsigned perpendicular;
} Movimento;

/* Construtor de um ponteiro para movimento.
 *  Recebe número variável de argumentos, pois operações diferentes
 * têm necessidades diferentes.
 */
Movimento *novo_mov(Operacao op, ...);
/* Imprime o movimento na saída padrão. */
void imprime_mov(Movimento mov);

/* Vetor de ruas perpendiculares, que são consideradas pilhas de alegorias. */
typedef struct _ruas {
    Pilha *ruas; /* vetor */
    unsigned tam; /* tamanho */
} Ruas;
/* Construtor do vetor com tamanho (fixo). */
Ruas constroi_ruas(unsigned tam);
/* Destrutor. */
void destroi_ruas(Ruas ruas);

/* Aloca memória para uma alegoria, para ser empilhada. */
unsigned *nova_alegoria(unsigned alegoria);
/* Tenta encaixar uma alegoria em alguma paralela.
 * Retorna a perpendicular onde foi encaixada ou 0, se não foi possível.
 */
unsigned encaixa_alegoria(Ruas ruas, unsigned alegoria);
/* Tenta desfilar uma dada alegoria. Retorna de qual rua saiu ou 0, se não foi possível. */
unsigned desfila_proximo(Ruas ruas, unsigned alegoria);

/* Função recursiva de organização do desfile para montar a pilha de movimentos.
 *  Essa função foi feita de maneira recursiva para aproveitar outra pilha, a de chamada de funções,
 * para inverter a ordem de empilhamento dos movimentos e simular uma fila, alcançando o padrão de
 * leitura da entrada esperado.
 * 
 *  Recebe sempre a pilha de movimentos já construida, o vetor de ruas também já inicializado, a
 * quantidade de alegorias a analisar e um ponteiro para onde guardar qual a última alegoria que
 * desfilou. Além disso, cada iteração recebe quantas alegorias já foram lidas até então.
 * 
 *  Retorna se é possível acontecer o desfile.
 */
bool organiza_desfile(Pilha movimentos, Ruas ruas, unsigned qtd_alegorias, unsigned *ultima_alegoria, unsigned ja_lidas) {
    unsigned alegoria; /* qual a alegoria atual */
    unsigned perpend; /* qual a perpendicular envolvida */
    unsigned i; /* índice de laço */

    /* vetor de movimentos de saída de alegorias que aguardavam */
    /* o número de máximo de alegorias que aguardavam é o número de alegorias já lidas */
    Movimento *alegorias_aguardando = malloc(ja_lidas * sizeof(Movimento));
    unsigned qtd_alegorias_aguardando = 0;
    
    /* encerra a recursão */
    if (ja_lidas == qtd_alegorias) {
        free(alegorias_aguardando);
        /* desfile pode ser realizado */
        return true;
    }
    
    /* lê a alegoria atual */
    scanf("%u", &alegoria);

    /* tenta desfilar as alegorias das perpendiculares que já podem ir */
    /* enquanto der para tirar das ruas */
    while ((perpend = desfila_proximo(ruas, (*ultima_alegoria)+1)) > 0) {
        /* guarda o movimento de saída no vetor */
        alegorias_aguardando[qtd_alegorias_aguardando].alegoria = ++(*ultima_alegoria); 
        alegorias_aguardando[qtd_alegorias_aguardando].perpendicular = perpend;
        qtd_alegorias_aguardando++;
    }

    /* se a alegoria já é a próxima a desfilar */
    if (alegoria == *ultima_alegoria + 1) {
        /* analisa o resto */
        (*ultima_alegoria)++;
        if(! organiza_desfile(movimentos, ruas, qtd_alegorias, ultima_alegoria, ja_lidas+1)) {
            free(alegorias_aguardando);
            /* se não der o resto, desfile impossível */
            return false;
        }

        /* registra o desfile da atual */
        empilhar(movimentos, novo_mov(DESFILA, alegoria));
        /* e os desfiles das alegorias que estavam nas perpendiculares */
        for (i = qtd_alegorias_aguardando; i > 0; i--) {
            empilhar(movimentos, novo_mov(SAI, alegorias_aguardando[i-1].alegoria, alegorias_aguardando[i-1].perpendicular));
        }

        free(alegorias_aguardando);
        return true;
    }

    /* se for possível encaixar a alegoria em alguma perpendicular */
    perpend = encaixa_alegoria(ruas, alegoria);
    if (perpend > 0) {
        /* analisa o resto */
        if (! organiza_desfile(movimentos, ruas, qtd_alegorias, ultima_alegoria, ja_lidas+1)) {
            free(alegorias_aguardando);
            return false;
        }

        /* registra a entrada */
        empilhar(movimentos, novo_mov(ENTRA, alegoria, perpend));
        /* e os desfiles das alegorias que estavam nas perpendiculares */
        for (i = qtd_alegorias_aguardando; i > 0; i--) {
            empilhar(movimentos, novo_mov(SAI, alegorias_aguardando[i-1].alegoria, alegorias_aguardando[i-1].perpendicular));
        }

        free(alegorias_aguardando);
        return true;
    }
    /* se não der para desfilar, nem encaixar em uma rua*/
    
    free(alegorias_aguardando);
    /* não é possivel o desfile */
    return false;
}

int main(void) {
    unsigned n, r; /* número de alegorias e de perpendiculares */
    unsigned rua; /* índice do laço */
    unsigned ja_desfilaram; /* marca quantas alegorias já desfilaram */
    Pilha movimentos; /* pilha de movimentos das alegorias */
    Ruas ruas; /* vetor das pilhas das ruas perpendiculares */
    Movimento *mov; /* ponteiro para um movimento */

    scanf("%u %u", &n, &r);
    ruas = constroi_ruas(r);

    movimentos = constroi_pilha();

    ja_desfilaram = 0;

    /* análise dos movimentos */
    if (! organiza_desfile(movimentos, ruas, n, &ja_desfilaram, 0)) {
        printf("Sem desfile.\n");

    } else {
        printf("Desfile pode ser realizado:\n");
    }

    /* impressão dos movimentos */
    while ((mov = desempilhar(movimentos)) != NULL) {
        imprime_mov(*mov);
        free(mov);
    }

    /* impressão das alegorias que restaram nas perpendiculares */
    while ((rua = desfila_proximo(ruas, ++ja_desfilaram)) > 0) {
        Movimento movi;
        movi.operacao = SAI;
        movi.alegoria = ja_desfilaram;
        movi.perpendicular = rua;

        imprime_mov(movi);
    }

    /* libera memória e encerra */
    destroi_pilha(movimentos, free);
    destroi_ruas(ruas);
    return 0;
}

Movimento *novo_mov(Operacao op, ...) {
    Movimento *novo;

    /* argumentos recebidos após o primeiro */
    va_list args; va_start(args, op);

    novo = malloc(sizeof(struct _mov));
    novo->operacao = op;

    switch (op) {
        case DESFILA: /* recebe apenas um argumento adicional */
            novo->alegoria = va_arg(args, unsigned);
        break;

        case ENTRA: /* recebem dois argumentos adicionais */
        case SAI:
            novo->alegoria = va_arg(args, unsigned);
            novo->perpendicular = va_arg(args, unsigned);
        break;
    }

    return novo;
}
void imprime_mov(Movimento mov) {
    switch (mov.operacao) {
        case DESFILA:
            printf("Alegoria %u desfila.\n", mov.alegoria);
        break;
        case ENTRA:
            printf("Alegoria %u entra na rua %u e aguarda.\n", mov.alegoria, mov.perpendicular);
        break;
        case SAI:
            printf("Alegoria %u sai da rua %u e desfila.\n", mov.alegoria, mov.perpendicular);
        break;
    }
}

Ruas constroi_ruas(unsigned tam) {
    unsigned i; /* índice do laço */

    Ruas novas;
    novas.tam = tam;
    /* vetor de perpendiculares */
    novas.ruas = malloc(tam * sizeof(Pilha));

    /* inicializa cada rua */
    for (i = 0; i < tam; i++) {
        novas.ruas[i] = constroi_pilha();
    }

    return novas;
}
void destroi_ruas(Ruas ruas) {
    unsigned i; /* índice */

    /* destroi cada pilha */
    for (i = 0; i < ruas.tam; i++) {
        destroi_pilha(ruas.ruas[i], free);
    }
    /* e, por fim, o vetor */
    free(ruas.ruas);
}

unsigned *nova_alegoria(unsigned alegoria) {
    unsigned *nova = malloc(sizeof(int));
    *nova = alegoria;
    return nova;
}
unsigned encaixa_alegoria(Ruas ruas, unsigned alegoria) {
    /* para cada rua */
    unsigned i; for (i = 0; i < ruas.tam; i++) {
        /* vê a última alegoria da rua */
        unsigned *aleg_aguardando = ver_topo(ruas.ruas[i]);
        
        /* se não tiver nenhuma alegoria ou alegoria que tem desfila depois */
        if (aleg_aguardando == NULL || *aleg_aguardando > alegoria) {
            /* coloca a nova alegoria da perpendicular */
            empilhar(ruas.ruas[i], nova_alegoria(alegoria));
            /* e retorna a rua */
            return i+1;
        }
    }

    /* sem ruas possíveis */
    return 0;
}
unsigned desfila_proximo(Ruas ruas, unsigned alegoria) {
    /* em cada rua */
    unsigned i; for (i = 0; i < ruas.tam; i++) {
        /* olha a última a alegoria a entrar na rua */
        unsigned *proxima_aleg = ver_topo(ruas.ruas[i]);

        /* se tiver alguma coisa e for a alegoria buscada */
        if (proxima_aleg != NULL && *proxima_aleg == alegoria) {
            /* remove da rua */
            free(desempilhar(ruas.ruas[i]));
            /* e retorna a perpendicular onde estava */
            return i+1;
        }
    }

    /* alegoria não encontrada */
    return 0;
}