#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "fila.h"
#include "participante.h"

#define pow2(n) \
    (1 << n)

#define troca(p1, p2) \
    p1 ^= p2 ^= p1 ^= p2

#define min_id(a, b) \
    ((pegar_id(a) < pegar_id(b))? pegar_id(a) : pegar_id(b))

#define max_id(a, b) \
    ((pegar_id(a) > pegar_id(b))? pegar_id(a) : pegar_id(b))

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
    if (perdedores != NULL) {
        enfileirar(perdedores, perdedor);
    } else {
        destroi_part(perdedor);
    }
}

int proxima_fase(Fila ganhadores, Fila perdedores, int historico) {
    int partidas = tamanho_fila(ganhadores) / 2;
    for (int i = 1; i <= partidas; i++) {
        Participante part1 = desenfileirar(ganhadores);
        Participante part2 = desenfileirar(ganhadores);

        Participante ganhou = (partida(part1, part2) == part1)? part1 : part2;
        Participante perdeu = (ganhou == part1)? part2 : part1;

        enfileirar(ganhadores, ganhou);
        if (perdedores != NULL) {
            printf("Partida %d: %d vs %d venceu %d\n", historico+i, min_id(part1, part2), max_id(part1, part2), pegar_id(ganhou));
            enfileirar(perdedores, perdeu);
        } else {
            printf("Partida %d da repescagem: %d vs %d venceu %d\n", historico+i, min_id(part1, part2), max_id(part1, part2), pegar_id(ganhou));
            destroi_part(perdeu);
        }
    }

    return partidas;
}

void novo_dia(const void *part, va_list regen) {
    proximo_dia((Participante) part, va_arg(regen, int));
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
        partidas_principais += proxima_fase(principal, repescagem, partidas_principais);

        partidas_repescagem += proxima_fase(repescagem, NULL, partidas_repescagem);

        atualizar_dados(principal, novo_dia, K);
        atualizar_dados(repescagem, novo_dia, K);
    }

    while ((partidas_repescagem += proxima_fase(repescagem, NULL, partidas_repescagem)) > 0);

    Participante part1 = desenfileirar(principal);
    Participante part2 = desenfileirar(repescagem);

    Participante ganhou = (partida(part1, part2) == part1)? part1 : part2;
    printf("Final do torneio: %d vs %d campeao %d\n", min_id(part1, part2), max_id(part1, part2), pegar_id(ganhou));

    destroi_part(part1);
    destroi_part(part2);

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