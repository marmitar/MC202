/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 06
 */

/*****************************************
 * Implementação da fila de prioridades. *
 *****************************************/
#include "fila_prioridade.h"

#include <stdlib.h>
#include <stdbool.h>

struct _max_heap {
    Item *heap;
    unsigned tamanho;
    unsigned capacidade;

    Destrutor destrutor;
    Comparador comparador;
};

/* ponteiro vazio */
#define eh_vazio(ptr) \
    (ptr == NULL)

#define pai(i) \
    (((int) i-1) / 2)
#define filho_esq(i) \
    (2*i + 1)
#define filho_dir(i) \
    (2*i + 2)

#define troca_com_pai(maxheap, filho) \
    Item pai = maxheap->heap[pai(filho)]; \
    maxheap->heap[pai(filho)] = maxheap->heap[filho]; \
    maxheap->heap[filho] = pai 

/* compara duas posições do heap */
#define compara(maxheap, i, j) \
    maxheap->comparador(maxheap->heap[i], maxheap->heap[j])

/* teste se um nó é maior que o pai */
#define maior_que_pai(maxheap, filho) \
    (compara(maxheap, filho, pai(filho)) > 0)

/* encontra o maior item entre o pai e os dois filhos */
#define maior_item(maxheap, pai) \
    ((compara(maxheap, filho_dir(pai), pai) > 0)? \
        ((compara(maxheap, filho_dir(pai), filho_esq(pai)) > 0)? \
            filho_dir(pai) : \
            ((compara(maxheap, filho_esq(pai), pai) > 0)? \
                filho_esq(pai) : \
                filho_dir(pai) \
            ) \
        ) : \
        ((compara(maxheap, filho_esq(pai), pai) > 0 )? \
            filho_esq(pai) : pai \
        ) \
    )

FilaPrio constroi_filaprio(unsigned capacidade_inicial, Comparador comparador, Destrutor destrutor) {
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

    nova->comparador = comparador;
    nova->destrutor = destrutor;
    return nova;
}

unsigned destroi_filaprio(FilaPrio maxheap) {
    unsigned destruidos = 0;
    unsigned i;

    if (eh_vazio(maxheap)) {
        return destruidos;
    }

    if (eh_vazio(maxheap->heap)) {
        free(maxheap);
        return destruidos;
    }

    for (i = 0; i < maxheap->tamanho; i++) {
        maxheap->destrutor(maxheap->heap[i]);
        destruidos++;
    }

    free(maxheap->heap);
    free(maxheap);
    return destruidos;
}

bool inserir_item(FilaPrio maxheap, Item item) {
    unsigned k;

    if (eh_vazio(maxheap)) {
        return false;
    }

    /* duplica a capacidade, se necessário */
    if (maxheap->tamanho == maxheap->capacidade) {
        Item *novo_heap;
        maxheap->capacidade *= 2;
        novo_heap = (Item *) realloc(maxheap->heap, maxheap->capacidade);

        if (eh_vazio(novo_heap)) {
            return false;
        }

        maxheap->heap = novo_heap;
    }

    /* insere item */
    maxheap->heap[maxheap->tamanho++] = item;

    /* e sobe no heap o novo elemento */
    for (k = maxheap->tamanho-1; k > 0 && maior_que_pai(maxheap, k); k = pai(k)) {
        troca_com_pai(maxheap, k);
    }

    return true;
}

/* remove o maior elemento */
Item pegar_proximo(FilaPrio maxheap) {
    Item proximo;
    unsigned k, prox_k;

    if (eh_vazio(maxheap) || eh_vazio(maxheap->heap) || maxheap->tamanho == 0) {
        return NULL;
    }

    /* retira o maior elemento */
    proximo = maxheap->heap[0];
    /* coloca o último no lugar */
    maxheap->heap[0] = maxheap->heap[--maxheap->tamanho];

    /* se tiver um ou nenhum elemento, não precisa de ajuste */
    if (maxheap->tamanho < 2) {
        return proximo;
    }

    /* senão, desce no heap */
    k = 0;
    /* desce até o último nó com dois filhos
     * que é até onde maior_item é garantido o funcionamento
     */
    for (prox_k = maior_item(maxheap, 0); prox_k <= pai(maxheap->tamanho-2)
            && k != prox_k; prox_k = maior_item(maxheap, k)) {
        troca_com_pai(maxheap, prox_k);
        k = prox_k;
    }
    /* e então desce para o último nível */
    if (prox_k < maxheap->tamanho && k != prox_k) {
        troca_com_pai(maxheap, prox_k);
        k = prox_k;
        
        /* o filho na verdade pode ser o único elemento com apenas um filho,
         * se esse elemento existir, e nesse caso ainda existe esse filho
         * para ser testado
         */
        prox_k = filho_dir(k);
        if (prox_k < maxheap->tamanho && compara(maxheap, prox_k, k) > 0) {
            troca_com_pai(maxheap, prox_k);
        }
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
