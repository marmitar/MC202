#ifndef _ARV_BIN_H
#define _ARV_BIN_H

/********************
 * ÁRVORES BINÁRIAS *
 ********************/

#include <stddef.h>
#include <stdbool.h>

/* Ordem de percurso na Árvore */
typedef enum {INORDEM, PREORDEM, POSORDEM} Ordem;

typedef void *Dado;
typedef int (*Comparador)(Dado, Dado);

typedef struct _arv_bin *Arvore;

/*  Recebe uma lista com os dados e constrói uma árvore binária
 * com eles de acordo com a ordem de percurso dada.
 */
Arvore constroi_de_lista(Dado *lista, size_t tamanho, Ordem);

void destroi_arvore(Arvore);

/* Testa se um certo dade existe na árvore. */
bool tem_elemento(Arvore, Dado, Comparador);

/*  Elementos de uma intersecção. Contém o dado e a
 * altura onde foi encontrado, na árvore em questão.
 */
typedef struct _elem {
    Dado dado;
    size_t altura;
} *Elemento;
/* Destrói lista de elementos de intersecção, como a retornada abaixo. */
void destroi_lista(Elemento *lista);

/*  Intersecção etre duas árvores. As alturas são em relação a árvore 'a'.
 * O elemento de intersecção retornado funciona como uma string, no final
 * existe um elemento vazio apenas para indicar o encerramento.
 */
Elemento *interseccao(Arvore a, Arvore b, Comparador, Ordem);

#endif