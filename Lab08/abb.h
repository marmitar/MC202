/* Tiago de Paula Alves - 187679 - MC202 A - Lab. 8 */

#ifndef _ABB_H
#define _ABB_H

/*****************************************************
 * INTERFACE DA BIBLITECA DE ÁRVORE BINÁRIA DE BUSCA *
 *****************************************************/

typedef struct _abb *ABB;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Ponteiro do elemento usado na ABB e o cabeçalho de suas funções, *
 * quem devem ser implementadas pelo usuário.                       *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
typedef void *Elemento;
/* Compara dois elementos e retorna um inteiro maior, igual ou menor
 * que zero se o primeiro elemento for, respectivamente, maior, igual
 * ou menor que o segundo.
 */
typedef int (*Comparador)(Elemento, Elemento);
/* Imprime o elemento. */
typedef void (*Impressor)(Elemento);
/* Trata da memória usada pelo elemento. */
typedef void (*Destrutor)(Elemento);


/* * * * * * * * * * * * * * * * *
 * Construtor e destrutor da ABB. *
  * * * * * * * * * * * * * * * * */
ABB constroi_abb(Comparador, Impressor, Destrutor);
void destroi_abb(ABB);


/* * * * * * * * * * * * * *
 * Funcionalidades da ABB. *
 * * * * * * * * * * * * * */
void insere_elemento(ABB, Elemento);
void remove_elemento(ABB, Elemento);
/* Impressão de todos os elementos x da ABB que pertencem ao
 * intervalo: incio <= x < fim.
 */
void imprime_intervalo(ABB, Elemento inicio, Elemento fim);

#endif