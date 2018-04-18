/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 04
 */

/* Implementação da lógica do participante do torneio */
#include "participante.h"

#include <stdlib.h>

/* participante */
struct _part {
    unsigned id; /* número de identificação no torneio */
    unsigned hab_ini; /* habilidade inicial */
    unsigned hab; /* habilidade atual */
};

/* retira o mínimo, escopo local */
#define min(a, b) \
    ((a < b)? a : b)

/* testa se o primeiro participante ganha do segundo em uma partida */
#define primeiro_ganha(p1, p2) \
    (p1->hab > p2->hab || (p1->hab == p2->hab && p1->id < p2->id))

/* Construtor e destrutor de participantes. */
Participante constroi_part(unsigned id, unsigned habilidade) {
    Participante novo = (Participante) malloc(sizeof(struct _part));
    novo->id = id;
    novo->hab = novo->hab_ini = habilidade;
    return novo;
}
void destroi_part(Participante part) {
    free(part);
}

/* Confronto entre dois participantes. */
Participante partida(Participante part1, Participante part2) {
    Participante ganhador, perdedor;

    /* seleciona o ganhador e o perdedor */
    if (primeiro_ganha(part1, part2)) {
        ganhador = part1;
        perdedor = part2;
    } else {
        ganhador = part2;
        perdedor = part1;
    }

    /* ajusta suas habilidades */
    ganhador->hab -= perdedor->hab;
    perdedor->hab /= 2;

    /* e retorna o ganhador */
    return ganhador;
}

/* Identificação do participante. */
unsigned pegar_id(Participante part) {
    return part->id;
}

/* Realiza o descanso do participante. */
void descanso(Participante part, unsigned regeneracao) {
    /* regeneração total */
    if (regeneracao == REGENERACAO_TOTAL) {
        part->hab = part->hab_ini;
    /* regeneração normal */
    } else {
        /* limitada pela habilidade inicial */
        part->hab = min(part->hab_ini, part->hab + regeneracao);
    }
}

#undef min
#undef primeiro_ganha
