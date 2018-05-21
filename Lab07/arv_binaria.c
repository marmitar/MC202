#include "arv_binaria.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define eh_vazio(ptr) \
    (ptr == NULL)

static No _rec_constroi_arvore(Item *lista, size_t inicio, size_t fim, Ordem ordem) {
    No novo = calloc(1, sizeof(struct _no));
    size_t meio;

    if (inicio == fim) {
        novo->dado = lista[inicio];
    }

    if (inicio >= fim) {
        return novo;
    }

    switch(ordem) {
        case PREORDEM:
            meio = (inicio + fim + 1)/2;
            novo->dado = lista[inicio];
            novo->esquerda = _rec_constroi_arvore(lista, inicio+1, meio, ordem);
            novo->direita = _rec_constroi_arvore(lista, meio+1, fim, ordem);
        break;
        case POSORDEM:
            meio = (inicio + fim - 1)/2;
            novo->dado = lista[fim];
            novo->esquerda = _rec_constroi_arvore(lista, inicio, meio, ordem);
            novo->direita = _rec_constroi_arvore(lista, meio+1, fim-1, ordem);
        break;
        case INORDEM:
            meio = (inicio + fim)/2;
            novo->dado = lista[meio];
            novo->esquerda = _rec_constroi_arvore(lista, inicio, meio-1, ordem);
            novo->direita = _rec_constroi_arvore(lista, meio+1, fim, ordem);
    }

    return novo;
}

Arvore constroi_arvore(Item *lista, size_t tamanho, Ordem ordem, Comparador comparador) {
    Arvore nova = malloc(sizeof(struct _arv_b));

    nova->comparador = comparador;
    nova->raiz = _rec_constroi_arvore(lista, 0, tamanho-1, ordem);
    return nova;
}

static unsigned _destroi_no(No raiz) {
    unsigned destruidos = 0;

    if (eh_vazio(raiz)) {
        return 0;
    }

    destruidos += _destroi_no(raiz->direita);
    destruidos += _destroi_no(raiz->esquerda);

    free(raiz);
    destruidos += 1;

    return destruidos;
}

unsigned destroi_arvore(Arvore arvore) {
    unsigned destruidos = _destroi_no(arvore->raiz);
    arvore->raiz = NULL;
    free(arvore);

    return destruidos;
}

static int rec_buscar_item(Comparador comparador, No raiz, Item item) {
    int altura;

    if (eh_vazio(raiz)) {
        return -1;
    }

    if (comparador(raiz->dado, item) == 0) {
        return 0;
    }

    altura = rec_buscar_item(comparador, raiz->esquerda, item);
    if (altura >= 0) {
        return altura + 1;
    }

    altura = rec_buscar_item(comparador, raiz->direita, item);
    if (altura >= 0) {
        return altura + 1;
    }

    return -1;
}

int buscar_item(Arvore arvore, Item item) {
    return rec_buscar_item(arvore->comparador, arvore->raiz, item);
}

#undef eh_vazio
#undef erro
#undef teste_erro
#undef teste_aloc
#undef teste_arg
