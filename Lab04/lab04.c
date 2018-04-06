#include <stdio.h>
#include <stdbool.h>

#include "fila.h"
#include "participante.h"

#define pow2(n) \
    (1 << n)

#define eh_vazio(ptr) \
    (ptr == NULL)

void ler_participantes(unsigned escala, Fila participantes) {
    unsigned total = pow2(escala);

    for (int i = 1; i <= total; i++) {
        int habilidade;
        scanf("%d", &habilidade);
        enfileirar(participantes, constroi_part(i, habilidade));
    }
}

void registra_resultado(Fila ganhadores, Fila perdedores, Participante ganhador, Participante perdedor) {
    printf(" venceu %d\n", pegar_id(ganhador));

    enfileirar(ganhadores, ganhador);
    if (! eh_vazio(perdedores)) {
        enfileirar(perdedores, perdedor);
    } else {
        destroi_part(perdedor);
    }
}

int proxima_fase(Fila ganhadores, Fila perdedores, int regen, int historico) {
    int partidas = tamanho_fila(ganhadores) / 2;
    for (int i = 1; i <= partidas; i++) {
        Participante part1 = desenfileirar(ganhadores);
        Participante part2 = desenfileirar(ganhadores);

        Participante ganhou = (partida(part1, part2, regen) == part1)? part1 : part2;
        Participante perdeu = (ganhou == part1)? part2 : part1;

        enfileirar(ganhadores, ganhou);
        if (! eh_vazio(perdedores)) {
            printf("Partida %d: %d vs %d venceu %d\n", historico+i, pegar_id(part1), pegar_id(part2), pegar_id(ganhou));
            enfileirar(perdedores, perdeu);
        } else {
            printf("Partida %d da repescagem: %d vs %d venceu %d\n", historico+i, pegar_id(part1), pegar_id(part2), pegar_id(ganhou));
            destroi_part(perdeu);
        }
    }

    return partidas;
}

int main(void) {
    int N, K;
    scanf("%d %d", &N, &K);

    Fila principal = constroi_fila();
    Fila repescagem = constroi_fila();

    ler_participantes(N, principal);

    int partidas_principais = 0;
    int partidas_repescagem = 0;
    for (int dia = 1; dia <= N; dia++) {
        partidas_principais += proxima_fase(principal, repescagem, K, partidas_principais);

        partidas_repescagem += proxima_fase(repescagem, NULL, K, partidas_repescagem);
    }

    while (proxima_fase(repescagem, NULL, K, partidas_repescagem) > 0);

    Participante part1 = desenfileirar(principal);
    Participante part2 = desenfileirar(principal);

    Participante ganhou = (partida(part1, part2, K) == part1)? part1 : part2;
    printf("Final do torneio: %d vs %d campeao %d\n", pegar_id(part1), pegar_id(part2), pegar_id(ganhou));

    if (tamanho_fila(principal) > 0) {
        fprintf(stderr, "PRINCIPAL NÃO VAZIA\n");
        return 1;
    }
    if (tamanho_fila(repescagem) > 0) {
        fprintf(stderr, "REPESCAGEM NÃO VAZIA\n");
        return 2;
    }

    destroi_fila(principal, NULL);
    destroi_fila(repescagem, NULL);
    return 0;
}