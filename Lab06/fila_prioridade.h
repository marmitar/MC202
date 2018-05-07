/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 06
 */

/*************************************
 * Interface da fila de prioridades. *
 *************************************/
#ifndef _FILA_PRIORIDADE_H
#define _FILA_PRIORIDADE_H

#include <stdbool.h>

/* tipo de fila de prioridades,
 * implementada em um heap de máximo
 */
typedef struct _max_heap *FilaPrio;

/* tipo usado na fila, normalmente um ponteiro genérico
 * mas pode ser definido pelo usuário como outro tipo,
 * caso seja usado apenas para esse tipo específico
 */
#ifndef TIPO
#define TIPO void *
#endif
typedef TIPO Item;

/* funções de tratamento do tipo usado */
/* O comparador deve receber dois itens e devolver '0'
 * se forem iguais, um inteiro positivo se o primeiro
 * for maior ou um negativo se o segundo for maior.
 */
typedef int (*Comparador)(Item, Item);
typedef void (*Destrutor)(Item);

/***************************
 * Construtor e destrutor. *
 ***************************/
FilaPrio constroi_filaprio(unsigned capacidade_inicial, Comparador, Destrutor);
unsigned destroi_filaprio(FilaPrio);

/********************************************
 * Funcionalidades da PILHA!!! de prioridades. *
 ********************************************/
bool inserir_item(FilaPrio, Item);
Item pegar_proximo(FilaPrio);

#endif
