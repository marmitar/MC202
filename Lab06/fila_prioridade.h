/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 06
 */

/**************************************************
 * Interface da fila de prioridades de processos. *
 **************************************************/
#ifndef _FILA_PRIORIDADE_H
#define _FILA_PRIORIDADE_H

#include <stdbool.h>
#include <stdarg.h>

typedef struct _max_heap *FilaPrio;

#ifndef TIPO
#define TIPO void *
#endif
typedef TIPO Item;
typedef int (*Comparador)(Item, Item);
typedef Item (*Construtor)(va_list);
typedef void (*Destrutor)(Item);

/***************************
 * Construtor e destrutor. *
 ***************************/
FilaPrio constroi_filaprio(unsigned capacidade_inicial, Construtor, Comparador, Destrutor);
unsigned destroi_filaprio(FilaPrio);

/********************************************
 * Funcionalidades da PILHA!!! de prioridades. *
 ********************************************/
bool novo_item(FilaPrio, ...);
// bool inserir_item(FilaPrio, Item);
Item pegar_proximo(FilaPrio);
Item ver_proximo(FilaPrio);
bool esta_vazia(FilaPrio);

#endif
