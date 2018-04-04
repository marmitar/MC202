/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 03
 */

/* Interface de uso da biblioteca de um simulador de memória para o laboratório. */
#ifndef _MEMORIA_H
#define _MEMORIA_H

#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

/* tipo de posição de memória, 64 bits com valor absoluto */
typedef uint_fast64_t Mem_pos;

/* estrutura de um segmento de memória */
typedef struct _seg {
    Mem_pos endereco;
    Mem_pos tamanho;
} *Segmento;

/* estrutura da memória simulada (imutável) */
typedef const struct _mem {
    Lista segmentos; /* segmentos livres */
    Mem_pos tamanho;
} Memoria;

/*  Construtor e destrutor do gerenciador de memória. */
Memoria constroi_memoria(Mem_pos tamanho);
void destroi_memoria(Memoria mem);

/* Funções de acesso à memória. */
bool aloca_memoria(Memoria mem, Mem_pos tamanho);
bool desaloca_memoria(Memoria mem, Mem_pos endereco, Mem_pos tamanho);
bool realoca_memoria(Memoria mem, Mem_pos endereco, Mem_pos tamanho, Mem_pos novo_tamanho);

/* Funções de análise da memória. */
void imprime_segmentos(Memoria mem);

#endif
