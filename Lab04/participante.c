#include "participante.h"

#include <stdlib.h>

struct _part {
    unsigned id;
    unsigned hab_ini;
    unsigned hab;
};

#define min(a, b) \
    ((a < b)? a : b)

#define primeiro_ganha(p1, p2) \
    (p1->hab > p2->hab || (p1->hab == p2->hab && p1->id < p2->id))

Participante constroi_part(unsigned id, unsigned habilidade) {
    Participante novo = (Participante) malloc(sizeof(struct _part));
    novo->id = id;
    novo->hab = novo->hab_ini = habilidade;
    return novo;
}
void destroi_part(Participante part) {
    free(part);
}

Participante partida(Participante part1, Participante part2) {
    Participante ganhador, perdedor;
    if (primeiro_ganha(part1, part2)) {
        ganhador = part1;
        perdedor = part2;
    } else {
        ganhador = part2;
        perdedor = part1;
    }

    ganhador->hab -= perdedor->hab;
    perdedor->hab /= 2;

    return ganhador;
}
unsigned pegar_id(Participante part) {
    return part->id;
}
void proximo_dia(Participante part, unsigned regeneracao) {
    part->hab = min(part->hab_ini, part->hab + regeneracao);
}

#undef min
#undef primeiro_ganha