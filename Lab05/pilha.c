/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 05
 */

/* Implementação de uma pilha genérica */
#include "pilha.h"

#include <stdlib.h>
#include <stdbool.h>

/* Registro de um nó, desconhecido do usuário da biblioteca. */
typedef struct _no {
    void *dado;
    struct _no *prox;
} *No;

/* Registro de uma pilha, desconhecido, porém acessível pelo usuário. */
struct _pilha {
    No topo;
};

/* Testa se o ponteiro não está vazio. */
#define eh_vazio(ptr) \
    (ptr == NULL)

/* Construtor. */
Pilha constroi_pilha(void) {
    return (Pilha) calloc(1, sizeof(struct _pilha));
}
/* Destrutor. Retorna quantos nós foram removidos. */
unsigned destroi_pilha(Pilha pilha, void (*desaloca)(void *dado)) {
    unsigned destruidos;
    No ptr;

    if (eh_vazio(pilha)) {
        return 0;
    }

    destruidos = 0;
    /* para cada nó */
    ptr = pilha->topo;
    while (! eh_vazio(ptr)) {
        No aux = ptr->prox;
        /* remove o dado */
        desaloca(ptr->dado);
        /* remove o nó */
        free(ptr);
        /* vai pro próximo */
        ptr = aux;
        /* e contabiliza */
        destruidos++;
    }

    /* libera a memória da pilha, em si */
    pilha->topo = NULL; /* evitar reutilização */
    free(pilha);

    /* retorna os nós destruidos */
    return destruidos;
}

/* Guarda o novo dado no topo da pilha. */
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

    /* guarda os dados */
    no->dado = dado;
    /* e insere o nó */
    no->prox = pilha->topo;
    pilha->topo = no;

    return true; /* empilhamento com êxito */
}
/* Retira o dado no topo da pilha. */
void *desempilhar(Pilha pilha) {
    No ptr;
    void *dado;

    /* pilha inexiste ou vazia, sem dado para desempilhar */
    if (eh_vazio(pilha) || eh_vazio(pilha->topo)) {
        return NULL;
    }

    /* pega o dado */
    ptr = pilha->topo;
    dado = (void *) ptr->dado;
    /* ajusta a pilha */
    pilha->topo = pilha->topo->prox;
    /* libera o nó */
    free(ptr);
    /* e devolve o dado */
    return dado;
}

/* Vê o dado no topo da pilha. */
void *ver_topo(Pilha pilha) {
    /* pilha inexiste ou vazia, sem dado para desempilhar */
    if (eh_vazio(pilha) || eh_vazio(pilha->topo)) {
        return NULL;
    }

    return pilha->topo->dado; /* dado do topo da pilha */
}