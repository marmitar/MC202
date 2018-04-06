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
    (p1->hab > p2->hab || (p1->hab == p2->hab && p1->id > p2->id))

Participante constroi_part(unsigned id, unsigned habilidade) {
    Participante novo = (Participante) malloc(sizeof(struct _part));
    novo->id = id;
    novo->hab = novo->hab_ini = habilidade;
    return novo;
}
void destroi_part(Participante part) {
    free(part);
}

Participante partida(Participante part1, Participante part2, unsigned regeneracao) {
    Participante ganhador, perdedor;
    if (primeiro_ganha(part1, part2)) {
        ganhador = part1;
        perdedor = part2;
    } else {
        ganhador = part2;
        perdedor = part1;
    }

    ganhador->hab = min(ganhador->hab_ini, ganhador->hab - perdedor->hab + regeneracao);
    perdedor->hab = min(perdedor->hab_ini, perdedor->hab/2 + regeneracao);

    return ganhador;
}
unsigned pegar_id(Participante part) {
    return part->id;
}

#undef min