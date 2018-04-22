/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 05
 */

/************************************
 * Interface de uma pilha genérica. *
 ************************************/
#ifndef _PILHA_H
#define _PILHA_H

#include <stdlib.h>
#include <stdbool.h>

/*****************
 * Tipo 'Pilha'. *
 *****************/

/* Implentação desconhecida pelo usuário da biblioteca. */
typedef struct _pilha *Pilha;

/***************************
 * Construtor e destrutor. *
 ***************************/

/* Retorna a nova pilha. */
Pilha constroi_pilha(void);
/* A memória do tipo utilizado é liberada pela função apontada
 * por 'desaloca'. Retorna quantos nós foram removidos.
 */
unsigned destroi_pilha(Pilha pilha, void (*desaloca)(void *dado));

/********************
 * Funcionalidades. *
 ********************/

/* Retorna se foi possível empilhar (i.e. tem memória para tanto). */
bool empilhar(Pilha pilha, void *dado);
/* Retorna o dado (ponteiro) do topo da pilha, ou NULL em caso de falha. */
void *desempilhar(Pilha pilha);

/* Vê o dado do topo da pilha, sem removê-lo. */
void *ver_topo(Pilha pilha);

#endif