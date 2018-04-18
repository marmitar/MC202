/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 04
 */

/* Implementação de uma fila genérica */
#include "fila.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

/* A fila é implementada em uma lista duplamente ligada. */
struct _no {
    const void *dado; /* dado genérico */
    struct _no *prox; /* próximo */
    struct _no *ant; /* anterior */
};
struct _fila {
    No ini; /* início */
    No fim; /* final */
    unsigned tam; /* tamanho */
};

/* testa se um ponteiro é vazio, escopo local */
#define eh_vazio(ptr) \
    (ptr == NULL)

/* contrutor do nó, escopo local */
#define constroi_no(novo, dado) \
    novo = (No) calloc(1, sizeof(struct _no)); \
    novo->dado = dado

/* Contrutor e destruto de filas. */
Fila constroi_fila(void) {
    /* é apenas um ponteiro para uma memória com apenas zeros */
    return (Fila) calloc(1, sizeof(struct _fila));
}
void destroi_fila(Fila fila, void (*desaloca)(const void *dado)) {
    /* desaloca cada nó */
    No ptr; for (ptr = fila->ini; ptr; ptr = ptr->prox) {
        desaloca(ptr->dado);
        free(ptr);
    }
    /* zera cada membro do registro, para evitar erros */
    fila->fim = fila->ini = NULL;
    fila->tam = 0;
    /* e libera sua memória */
    free(fila);
}

/* Funcionalidades básicas de uma fila. */
void enfileirar(Fila fila, const void *dado) {
    /* constroi e insere o nó */
    No novo = constroi_no(novo, dado);
    novo->prox = fila->ini;
    fila->ini = novo;

    /* ajusta os ponteiros da fila */
    if (! eh_vazio(fila->fim)) {
        fila->ini->prox->ant = novo;
    } else {
        fila->fim = novo;
    }
    /* e o seu tamanho */
    fila->tam++;
}
void *desenfileirar(Fila fila) {
    void *dado;

    /* lista vazia */
    if (eh_vazio(fila->fim)) {
        return NULL;
    }

    /* gurda o dado */
    dado = (void *) fila->fim->dado;
    /* remove o nó */
    if (! eh_vazio(fila->fim->ant)) {
        fila->fim = fila->fim->ant;
        free(fila->fim->prox);
        fila->fim->prox = NULL;
    } else {
        free(fila->fim);
        fila->fim = fila->ini = NULL;
    }
    /* ajusta o tamanho */
    fila->tam--;

    /* e retorna o dado */
    return dado;
}

/* Atualiza cada dado da fila. */
void atualizar_dados(Fila fila, void (atualiza)(const void *dado, va_list args), ...) {
    va_list args; /* argumentos passados */
    
    /* para cada nó */
    No ptr; for (ptr = fila->ini; ! eh_vazio(ptr); ptr = ptr->prox) {
        va_start(args, atualiza); /* (re)inicializa os argumentos */
        atualiza(ptr->dado, args); /* atualiza o dado do nó com os argumentos */
        va_end(args); /* encerra os argumentos */
    }
}

unsigned tamanho_fila(Fila fila) {
    return fila->tam;
}

/* encerra escopo das funções macro */
#undef eh_vazio
#undef constroi_no
