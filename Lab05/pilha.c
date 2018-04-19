#include "pilha.h"

#include <stdlib.h>
#include <stdbool.h>

typedef struct _no {
    const void *dado;
    struct _no *prox;
} *No;

struct _pilha {
    No topo;
};

#define eh_vazio(ptr) \
    (ptr == NULL)

#define constroi_no(noh, dado) \
    noh = (No) malloc(sizeof(struct _no)); \
    if (! eh_vazio(noh)) { \
        noh->dado = dado; \
    } (void) 0

Pilha constroi_pilha(void) {
    return (Pilha) calloc(1, sizeof(struct _pilha));
}
void destroi_pilha(Pilha pilha, void (*desaloca)(const void *dado)) {
    if (eh_vazio(pilha)) {
        return;
    }

    No ptr = pilha->topo;
    while (! eh_vazio(ptr)) {
        No aux = ptr->prox;
        desaloca(ptr->dado);
        free(ptr);
        ptr = aux;
    }

    pilha->topo = NULL;
    free(pilha);
}

bool empilhar(Pilha pilha, const void *dado) {
    if (eh_vazio(pilha)) {
        return false;
    }

    No no = (No) malloc(sizeof(struct _no));

    if (eh_vazio(no)) {
        return false;
    }

    no->dado = dado;
    no->prox = pilha->topo;
    pilha->topo = no;

    return true;
}
void *desempilhar(Pilha pilha) {
    if (eh_vazio(pilha) || eh_vazio(pilha->topo)) {
        return NULL;
    }

    No ptr = pilha->topo;
    void *dado = (void *) ptr->dado;

    pilha->topo = pilha->topo->prox;

    free(ptr);

    return dado;
}


void *pegar_proximo(Pilha pilha) {
    if (eh_vazio(pilha) || eh_vazio(pilha->topo)) {
        return NULL;
    }

    return (void *) pilha->topo->dado;
}