/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 02
 */

#ifndef VETOR_H
#define VETOR_H

#include <stdlib.h>
#include <stdbool.h>
#include "ordem.h"

/* Vetor dinâmico de ordens  */
typedef struct {
    int tamanho;
    int capacidade;

    Ordem *vetor;
} Vetor;

Vetor criar_vetor(void);
void destroi_vetor(Vetor *ordens);

int insere_ordem(Vetor *ordens, int identidade, int quantidade, double preco);
int remove_ordem(Vetor *ordens, int posicao);
int busca_ordem(Vetor *ordens, double preco);

/* testa se a ordem na posição especificada é válida e tem o preço desejado */
/* macro para ser mais rápida */
#define teste_preco(ordens, posicao, preco) \
    (posicao >= (ordens)->tamanho || posicao < 0? false : (ordens)->vetor[posicao].preco == preco)

#endif