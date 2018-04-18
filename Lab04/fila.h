/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 04
 */

/* Interface de uma fila genérica */
#ifndef _FILA_H
#define _FILA_H

#include <stdlib.h>
#include <stdarg.h> /* para uma lista variável de argumentos */

/*  Estruturas usadas, desconhecidas para o usuário.
 *  Todas as suas funcionalidades são implementadas nas funções abaixo.
 */
typedef struct _no *No;
typedef struct _fila *Fila;

/*  Construtor e destrutor de uma fila qualquer, em que 'desaloca' é um
 * ponteiro para uma função que cuida da memória do tipo usado para 'dado'.
 */
Fila constroi_fila(void);
void destroi_fila(Fila fila, void (*desaloca)(const void *dado));

/*  Funcionalidades básicas de uma fila, em que
 * 'dado' é um ponteiro para um tipo qualquer.
 */
void enfileirar(Fila fila, const void *dado);
void *desenfileirar(Fila fila);

/*  Percorre a fila, atualizando os dados de cada elemento.
 *  A função recebe um número variável de argumentos a partir da terceira posição, que são repassados em uma 'va_list', já
 * inicializada, para a função 'atualiza' de atualização de cada dado, que deve ser implementada pelo usuário da biblioteca.
 */
void atualizar_dados(Fila fila, void (atualiza)(const void *dado, va_list args), ...);

/* Tamanho da fila. */
unsigned tamanho_fila(Fila fila);

#endif
