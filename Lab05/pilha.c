/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 05
 */

/* Implementação de uma pilha genérica */
#include "pilha.h"

#include <stdlib.h>
#include <stdbool.h>

typedef struct _no {
    void *dado;
    struct _no *prox;
} *No;

struct _pilha {
    No topo;
};

/* Testa se o ponteiro representa vazio. */
#define eh_vazio(ptr) \
    (ptr == NULL)

Pilha constroi_pilha(void) {
    return (Pilha) calloc(1, sizeof(struct _pilha));
}

unsigned destroi_pilha(Pilha pilha, void (*desaloca)(void *dado)) {
    unsigned destruidos; /* conta quantos nós foram removidos */
    No ptr;

    if (eh_vazio(pilha)) {
        return 0;
    }

    destruidos = 0;
    ptr = pilha->topo;
    while (! eh_vazio(ptr)) {
        No aux = ptr->prox;
        desaloca(ptr->dado);
        free(ptr);
        ptr = aux;
        destruidos++;
    }

    pilha->topo = NULL; /* evitar reutilização */
    free(pilha);

    return destruidos;
}

bool empilhar(Pilha pilha, void *dado) {
    No no;

    /* não existe pilha para empilhar dado */
    if (eh_vazio(pilha)) {
        return false;
    }

    no = (No) malloc(sizeof(struct _no));
    /* não tem memória suficiente */
    if (eh_vazio(no)) {
        return false;
    }

    no->dado = dado;
    no->prox = pilha->topo;
    pilha->topo = no;

    return true;
}

void *desempilhar(Pilha pilha) {
    No ptr;
    void *dado;

    /* pilha inexiste ou é vazia, sem dado para desempilhar */
    if (eh_vazio(pilha) || eh_vazio(pilha->topo)) {
        return NULL;
    }

    ptr = pilha->topo;
    dado = (void *) ptr->dado;
    pilha->topo = pilha->topo->prox;
    free(ptr);
    return dado;
}

void *ver_topo(Pilha pilha) {
    /* pilha inexiste ou vazia, sem dado para ver */
    if (eh_vazio(pilha) || eh_vazio(pilha->topo)) {
        return NULL;
    }

    return pilha->topo->dado;
}