#include "fila.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

struct _no {
    const void *dado;
    struct _no *prox;
    struct _no *ant;
};

struct _fila {
    No ini; /* início da fila */
    No fim;
    unsigned tam;
};

#define eh_vazio(ptr) \
    (ptr == NULL)

static No constroi_no(const void *dado) {
    No novo = (No) calloc(1, sizeof(struct _no));
    novo->dado = dado;
    return novo;
}

Fila constroi_fila(void) {
    return (Fila) calloc(1, sizeof(struct _fila));
}
void destroi_fila(Fila fila, void (*desaloca)(const void *dado)) {
    for (No ptr = fila->ini; ptr; ptr = ptr->prox) {
        desaloca(ptr->dado);
        free(ptr);
    }
    free(fila);
}

void enfileirar(Fila fila, const void *dado) {
    No novo = constroi_no(dado);
    novo->prox = fila->ini;
    fila->ini = novo;

    if (! eh_vazio(fila->fim)) {
        fila->ini->prox->ant = novo;
    } else {
        fila->fim = novo;
    }

    fila->tam++;
}
void *desenfileirar(Fila fila) {
    if (eh_vazio(fila->fim)) {
        return NULL;
    }

    void *dado = (void *) fila->fim->dado;

    if (! eh_vazio(fila->fim->ant)) {
        fila->fim = fila->fim->ant;
        free(fila->fim->prox);
        fila->fim->prox = NULL;
    } else {
        free(fila->fim);
        fila->fim = fila->ini = NULL;
    }

    fila->tam--;
    return dado;
}

void atualizar_dados(Fila fila, void (atualiza)(const void *dado, va_list args), ...) {
    va_list args;
    for (No ptr = fila->ini; ! eh_vazio(ptr); ptr = ptr->prox) {
        va_start(args, atualiza);
        atualiza(ptr->dado, args);
        va_end(args);
    }
}

unsigned tamanho_fila(Fila fila) {
    return fila->tam;
}

#undef eh_vazio