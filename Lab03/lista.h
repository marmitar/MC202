/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 03
 */

/* Interface de uso da biblioteca de lista ligada ordenada genérica. */
#ifndef _LISTA_H
#define _LISTA_H

#include <stdlib.h>
#include <stdbool.h>

/* estrutura do nó da lista ligada */
typedef struct _no {
    const void *dado; /* ponteiro de tipo genérico, não pode ser reapontado */
    struct _no *prox; /* próximo nó */
} *No;
/* estrutura da lista */
typedef struct _lst {
    No cab; /* cabeça da lista */
} *Lista;

/* Inicializa uma lista ligada vazia.
 */
Lista constroi_lista(void);
/*  Libera a memória da lista.
 *  Usa a função 'desaloca' para desalocar o tipo utilizado. Deve ser implementada pelo usuário
 * para receber um ponteiro, que deverá ser tratado como o tipo utilizado, e liberar sua memória.
 */
void destroi_lista(Lista lista, void (*desaloca)(const void *dado));

/*  Insere um nó na lista de forma ordenada.
 *  Usa a função 'compara' para a ordenação. Deve ser implementada pelo usuário para receber os ponteiros
 * para os dados e devolver 0 se forem iguais, um inteiro positivo se o primeiro dado for considerado
 * maior e um positivo se o segundo for maior. 
 */
void insere_dado(Lista lista, const void *dado,  int (*compara)(const void *primeiro, const void *segundo));
/*  Remove o primeiro dado na lista igual ao argumento passado, retorna se ocorreu ou não a remoção.
 *  Usa uma função de comparação como em 'insere_dado' e uma de desalocação como em 'destroi_lista'.
 */
bool remove_dado(Lista lista, const void *dado, int (*compara)(const void *primeiro, const void *segundo), void (*desaloca)(const void *dado));

#endif
