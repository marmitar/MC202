/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 06
 */

/******************************************************
 * Implementação da fila de prioridades de processos. *
 ******************************************************/
#include "fila_prioridade.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

struct _max_heap {
    Item *heap;
    unsigned tamanho;
    unsigned capacidade;

    Construtor construtor;
    Destrutor destrutor;
    Comparador comparador;
};

#define eh_vazio(ptr) \
    (ptr == NULL)

#define pai(i) \
    ((i-1) / 2)

#define filho_esq(i) \
    (2*i + 1)
#define filho_dir(i) \
    (2*i + 2)

#define troca_com_pai(maxheap, filho) \
    Item pai = maxheap->heap[pai(filho)]; \
    maxheap->heap[pai(filho)] = maxheap->heap[filho]; \
    maxheap->heap[filho] = pai 

#define compara(maxheap, i, j) \
    maxheap->comparador(maxheap->heap[i], maxheap->heap[j])

#define maior_que_pai(maxheap, filho) \
    (compara(maxheap, filho, pai(filho)) > 0)

#define maior_filho(maxheap, pai) \
    ((compara(maxheap, filho_dir(pai), filho_esq(pai)) > 0)? filho_dir(pai) : filho_esq(pai))

FilaPrio constroi_filaprio(unsigned capacidade_inicial, Construtor construtor, Comparador comparador, Destrutor destrutor) {
    FilaPrio nova = (FilaPrio) malloc(sizeof(struct _max_heap));
    if (eh_vazio(nova)) {
        return NULL;
    }

    nova->capacidade = capacidade_inicial > 0? capacidade_inicial : 1;
    nova->heap = (Item *) malloc(nova->capacidade * sizeof(Item));
    if (eh_vazio(nova->heap)) {
        free(nova);
        return NULL;
    }

    nova->tamanho = 0;

    nova->construtor = construtor;
    nova->comparador = comparador;
    nova->destrutor = destrutor;
    return nova;
}

unsigned destroi_filaprio(FilaPrio maxheap) {
    unsigned destruidos = 0;

    if (eh_vazio(maxheap)) {
        return destruidos;
    }

    if (eh_vazio(maxheap->heap)) {
        free(maxheap);
        return destruidos;
    }

    for (unsigned i = 0; i < maxheap->capacidade; i++) {
        maxheap->destrutor(maxheap->heap[i]);
        destruidos++;
    }

    return destruidos;
}

bool inserir_item(FilaPrio maxheap, Item item) {
    if (eh_vazio(maxheap)) {
        return false;
    }

    if (maxheap->tamanho == maxheap->capacidade) {
        maxheap->capacidade *= 2;
        Item *novo_heap = (Item *) realloc(maxheap->heap, maxheap->capacidade);

        if (eh_vazio(novo_heap)) {
            return false;
        }

        maxheap->heap = novo_heap;
    }

    maxheap->heap[maxheap->tamanho++] = item;

    for (unsigned k = maxheap->tamanho-1; k > 0 && maior_que_pai(maxheap, k); k = pai(k)) {
        troca_com_pai(maxheap, k);
    }

    return true;
}

bool novo_item(FilaPrio maxheap, ...) {
    va_list argumentos;
    va_start(argumentos, maxheap);
    Item novo = maxheap->construtor(argumentos);
    va_end(argumentos);

    if (eh_vazio(novo)) {
        return false;
    }

    if (! inserir_item(maxheap, novo)) {
        maxheap->destrutor(novo);
        return false;
    }

    return true;
}

Item pegar_proximo(FilaPrio maxheap) {
    if (eh_vazio(maxheap) || eh_vazio(maxheap->heap) || maxheap->tamanho == 0) {
        return NULL;
    }

    Item proximo = maxheap->heap[0];
    maxheap->heap[0] = maxheap->heap[--maxheap->tamanho];

    unsigned k;
    for (k = maior_filho(maxheap, 0); k <= pai(maxheap->tamanho-2) && maior_que_pai(maxheap, k); k = maior_filho(maxheap, k)) {
        troca_com_pai(maxheap, k);
    }
    if (k < maxheap->tamanho && maior_que_pai(maxheap, k)) {
        troca_com_pai(maxheap, k);
    }

    return proximo;
}

#undef eh_vazio
#undef pai
#undef filho_esq
#undef filho_dir
#undef troca_com_pai
#undef compara
#undef maior_que_pai
#undef maior_filho
