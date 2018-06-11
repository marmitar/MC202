#ifndef _CONJHASH_H_
#define _CONJHASH_H_

/*****************
 * Conjunto Hash *
 *****************/
/* similar ao HashSet de Java, sendo que é uma Tabela de Hashing
 * sem repetições e apenas guarda os dados, em vez de usar
 * índices, além do próprio dado, para acessá-los.
 */

#include <stdbool.h>
#include <stddef.h>

/* Dado genérico e suas funções */
typedef void *dado_t;
typedef void (*Destrutor)(dado_t);
typedef int (*Comparador)(dado_t, dado_t);

/* ponteiro de um Conjunto Hash (HashSet) */
typedef struct _conj_hash *ConjHash;

ConjHash novo_conjhash(Destrutor, Comparador);
void del_conjhash(ConjHash);

/*  Insere elemento no Conjunto. Precisa do espaço
 * utilizado pelo dado para poder melhor distribuí-lo
 * na Tabela usada.
 * Retorna se foi possível inserir, uma vez que não é
 * possível repetir o elemento no conjunto.
 */
bool insere_elem(ConjHash, dado_t, size_t tam);

#endif