/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 04
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "fila.h"
#include "participante.h"

/* Leitura de 2^'escala' participantes pela entrada padrão. */
void ler_participantes(unsigned escala, Fila participantes);

/*  Regenera a habilidade de um participante apontado por 'part'.
 *  Feita para ser passada para a função 'atualizar_dados' de uma fila de participantes, que
 * deve ser chamada com apenas um argumento extra que é a capacidade de regeneração.
 * -> 'regen' é uma lista de argumentos, recebida de 'atualizar_dados', e deve conter apenas
 * um inteiro que é o valor da regeneração. 
 */
void restaura_hab(const void *part, va_list regen);

/*  Realiza as partidas de uma fase do torneio.
 * -> 'ganhadores': de onde vem os participantes e onde volta o vencedor;
 * -> 'perdedores': para onde vão os perdedores ou remoção deles se for NULL;
 * -> 'historico': quantidade de partidas que já ocorreu no torneio;
 * -> 'regeneracao': valor de regeneração de habilidade dos participantes;
 */
int proxima_fase(Fila ganhadores, Fila perdedores, int historico, int regeneracao);

/* Final do torneio. */
void final(Fila principal, Fila repescagem);

int main(void) {
    /* índice dos laços */
    int novas_partidas, dia;
    /* quantidades de partidas no torneio */
    int partidas_principais, partidas_repescagem;
    /* filas do torneio */
    Fila principal, repescagem;
    /* escala da quantidade de participantes e a capacidade de regeneração */
    int N, K; scanf("%d %d", &N, &K);

    /* inicializa as filas */
    principal = constroi_fila();
    repescagem = constroi_fila();

    /* lê a fila inicial do torneio principal */
    ler_participantes(N, principal);

    /* nenhuma partida ainda foi realizada */
    partidas_repescagem = partidas_principais = 0;

    /* para cada dia até N */
    for (dia = 1; dia <= N; dia++) {
        /* realiza a rodada principal */
        partidas_principais += proxima_fase(principal, repescagem, partidas_principais, K);
        /* e então a de repescagem */
        partidas_repescagem += proxima_fase(repescagem, NULL, partidas_repescagem, K);
    }

    /* até terminar a repescagem */
    while ((novas_partidas = proxima_fase(repescagem, NULL, partidas_repescagem, K)) > 0) {
        partidas_repescagem += novas_partidas;
    }

    /* rodada final */
    final(principal, repescagem);

    /* ERRO: sobraram elementos no torneio principal */
    if (tamanho_fila(principal) > 0) {
        fprintf(stderr, "PRINCIPAL NÃO VAZIA\n");
        return 1;
    }
    /* já a repescagem é forçada a terminar vazia */

    destroi_fila(principal, NULL);
    destroi_fila(repescagem, NULL);
    return 0;
}

/* calcula a n-ésima potência de 2 */
#define elev2(n) \
    (1 << n)

/* menor id entre dois participantes */
#define min_id(a, b) \
    ((pegar_id(a) < pegar_id(b))? pegar_id(a) : pegar_id(b))
/* maior id */
#define max_id(a, b) \
    ((pegar_id(a) > pegar_id(b))? pegar_id(a) : pegar_id(b))

void ler_participantes(unsigned escala, Fila participantes) {
    /* total de participantes */
    unsigned total = elev2(escala);
    
    /* para cada participante */
    int i; for (i = 1; i <= total; i++) {
        /* lê a sua habilidade */
        int habilidade; scanf("%d", &habilidade);
        /* e o armazena na fila do torneio */
        enfileirar(participantes, constroi_part(i, habilidade));
    }
}

void restaura_hab(const void *part, va_list regen) {
    descanso((Participante) part, va_arg(regen, int));
}

int proxima_fase(Fila ganhadores, Fila perdedores, int historico, int regeneracao) {
    /* quantidade de partidas realizadas */
    int partidas = tamanho_fila(ganhadores) / 2;
    int i;

    /* descanso antes das partidas de cada participante */
    atualizar_dados(ganhadores, restaura_hab, regeneracao);
    
    /* em cada partida da rodada */
    for (i = 1; i <= partidas; i++) {
        /* pega os dois pŕoximos participantes */
        Participante part1 = desenfileirar(ganhadores);
        Participante part2 = desenfileirar(ganhadores);

        /* realiza a partida e descobre o vencedor e o perdedor */
        Participante ganhou = partida(part1, part2);
        Participante perdeu = (ganhou == part1)? part2 : part1;

        /* volta o ganhador */
        enfileirar(ganhadores, ganhou);

        /* e move o perdedor (partida principal) */
        if (perdedores != NULL) {
            printf("Partida %d: %d vs %d venceu %d\n", historico+i, min_id(part1, part2), max_id(part1, part2), pegar_id(ganhou));
            enfileirar(perdedores, perdeu);
        /* ou remove completamente o perdedor (repescagem) */
        } else {
            printf("Partida %d da repescagem: %d vs %d venceu %d\n", historico+i, min_id(part1, part2), max_id(part1, part2), pegar_id(ganhou));
            destroi_part(perdeu);
        }
        /* realizando o registro */
    }

    /* retorna o total de partidas realizadas */
    return partidas;
}

void final(Fila principal, Fila repescagem) {
    Participante part1, part2, ganhador;

    /* descanso total do participante do principal */
    atualizar_dados(principal, restaura_hab, REGENERACAO_TOTAL);
    part1 = desenfileirar(principal);
    /* e do participante da repescagem */
    atualizar_dados(repescagem, restaura_hab, REGENERACAO_TOTAL);
    part2 = desenfileirar(repescagem);

    /* realiza e registra a partida */
    ganhador = partida(part1, part2);
    printf("Final do torneio: %d vs %d campeao %d\n", min_id(part1, part2), max_id(part1, part2), pegar_id(ganhador));

    /* destrói os últimos participantes */
    destroi_part(part1);
    destroi_part(part2);
}
