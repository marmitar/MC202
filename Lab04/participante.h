/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 04
 */

/* Interface da lógica do participante do torneio */
#ifndef _PARTICIPANTE_H
#define _PARTICIPANTE_H

#include <stdlib.h>
#include <limits.h>

/* Registro de um participante, implementação inacessível ao usuário. */
typedef struct _part *Participante;

/* Construtor e destrutor de participantes. */
Participante constroi_part(unsigned id, unsigned habilidade);
void destroi_part(Participante part);

/*  Realiza a partida entre dois participantes, ajusta suas
 * habilidades e retorna o ganhador entre eles.
 */
Participante partida(Participante part1, Participante part2);

/*  Retorna o número de identificação do participante,
 * passado no construtor pelo usuário da biblioteca. */
unsigned pegar_id(Participante part);

/* Lógica de descanso do participante. */
void descanso(Participante part, unsigned regeneracao);
/* valor de regeneração que garante descanso total do participante */
#define REGENERACAO_TOTAL UINT_MAX

#endif
