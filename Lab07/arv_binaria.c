#include "arv_binaria.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct _no {
    Item dado;
    struct _no *direita;
    struct _no *esquerda;
} *No;

struct _arv_b {
    Destrutor destrutor;
    Comparador comparador;
    No raiz;
    unsigned altura;
};

#define eh_vazio(ptr) \
    (ptr == NULL)

#define teste_erro(obj, ret, msg) \
    if (obj == NULL) { \
        fprintf(stderr, "ERRO: %s\n", msg); \
        return ret; \
    } (void) 0

#define teste_aloc(ptr, ret) \
    teste_erro(ptr, ret, "Não foi possível alocar memória.")

#define teste_arg(arg, ret) \
    teste_erro(arg, ret, "'" #arg "' inexistente.")

Arvore constroi_arvore(Destrutor destrutor, Comparador comparador) {
    teste_arg(comparador, NULL);

    Arvore nova = malloc(sizeof(struct _arv_b));
    teste_aloc(nova, NULL);

    nova->destrutor = destrutor;
    nova->comparador = comparador;
    nova->raiz = NULL;
    nova->altura = 0;
    return nova;
}

static unsigned _destroi_no(Destrutor destrutor, No raiz) {
    if (eh_vazio(raiz)) {
        return 0;
    }

    unsigned destruidos = 0;

    destruidos += _destroi_no(destrutor, raiz->direita);
    destruidos += _destroi_no(destrutor, raiz->esquerda);

    if (destrutor != NULL) {
        destrutor(raiz->dado);
    }
    free(raiz);
    destruidos += 1;

    return destruidos;
}

unsigned destroi_arvore(Arvore arvore) {
    teste_arg(arvore, 0);

    unsigned destruidos = _destroi_no(arvore->destrutor, arvore->raiz);
    arvore->raiz = NULL;
    arvore->altura = 0;
    free(arvore);

    return destruidos;
}

static bool _inserir_no(No raiz, No novo, const unsigned altura) {
    if (altura == 0) {
        return false;
    }

    if (eh_vazio(raiz->esquerda)) {
        raiz->esquerda = novo;
        return true;
    }

    if (eh_vazio(raiz->direita)) {
        raiz->direita = novo;
        return true;
    }

    if (_inserir_no(raiz->esquerda, novo, altura-1)) {
        return true;
    }

    if (_inserir_no(raiz->direita, novo, altura-1)) {
        return true;
    }

    return false;
}

bool inserir_item(Arvore arvore, Item item) {
    teste_arg(arvore, false);
    teste_arg(item, false);

    No novo = calloc(1, sizeof(struct _no));
    teste_aloc(novo, false);

    novo->dado = item;

    if (eh_vazio(arvore->raiz)) {
        arvore->raiz = novo;
        arvore->altura++;
        return true;
    }

    bool conseguiu;
    conseguiu = _inserir_no(arvore->raiz, novo, arvore->altura);
    if (conseguiu) {
        return true;
    }

    arvore->altura++;

    conseguiu = _inserir_no(arvore->raiz, novo, arvore->altura);
    if (conseguiu) {
        return true;
    }

    arvore->altura--;
    free(novo);
    return false;
}

static int rec_buscar_item(Comparador comparador, No raiz, Item item) {
    if (eh_vazio(raiz)) {
        return -1;
    }

    if (comparador(raiz->dado, item) == 0) {
        return 0;
    }

    int altura;

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
    teste_arg(arvore, -1);
    teste_arg(item, -1);

    return rec_buscar_item(arvore->comparador, arvore->raiz, item);
}

#undef eh_vazio
#undef teste_erro
#undef teste_aloc
#undef teste_arg
