#include "arv_treap.h"

#include <stdlib.h>
#include <time.h>
#include <limits.h>

#ifdef __unix__
#include <sys/random.h>
#endif

typedef long long unsigned Prio;

typedef struct _no {
    Dado dado;
    Prio pri;
    struct _no *esq, *dir;
} *No;
#define NO_VAZIO ((No) NULL)

struct _treap {
    No raiz;
    Comparador cmp;
    Destrutor destr;
};

Treap constroi_treap(Comparador comparador, Destrutor destrutor) {
    Treap novo = malloc(sizeof(struct _treap));
    novo->raiz = NO_VAZIO;
    novo->cmp = comparador;
    novo->destr = destrutor;
    return novo;
}

void _destroi_no_rec(Destrutor destroi, No raiz) {
    if (raiz == NO_VAZIO) {
        return;
    }

    _destroi_no_rec(destroi, raiz->esq);
    _destroi_no_rec(destroi, raiz->dir);

    destroi(raiz->dado);
    free(raiz);
}

void destroi_treap(Treap treap) {
    _destroi_no_rec(treap->destr, treap->raiz);
    free(treap);
}

static Prio _gerar_prio(void) {
#ifdef __unix__
    Prio nova;
    if (getrandom(&nova, sizeof(Prio), GRND_RANDOM | GRND_NONBLOCK) == sizeof(Prio)) {
        return nova;
    } else if (getrandom(&nova, sizeof(Prio), GRND_NONBLOCK) == sizeof(Prio)) {
        return nova;
    }
#endif

    srand(time(NULL));
    return rand();
}

static No _rotaciona_p_dir(No x) {
    No y = x->esq;

    x->esq = y->dir;
    y->dir = x;

    return y;
}

static No _rotaciona_p_esq(No x) {
    No y = x->dir;

    x->dir = y->esq;
    y->esq = x;

    return y;
}

static No _insere_no_rec(Comparador cmp, No raiz, No novo) {
    if (raiz == NULL) {
        return novo;
    }

    if (cmp(raiz->dado, novo->dado) > 0) {
        raiz->dir = _insere_no_rec(cmp, raiz->dir, novo);
        if (raiz->dir->pri > raiz->pri) {
            _rotaciona_p_esq(raiz);
        }
    } else {
        raiz->esq = _insere_no_rec(cmp, raiz->esq, novo);
        if (raiz->esq->pri > raiz->pri) {
            _rotaciona_p_dir(raiz);
        }
    }

    return raiz;
}

void insere_dado(Treap treap, Dado dado) {
    No novo = malloc(sizeof(struct _no));
    novo->dado = dado;
    novo->pri = _gerar_prio();

    treap->raiz =_insere_no_rec(treap->cmp, treap->raiz, novo);
}

No _pega_min_rec(No raiz, Dado *resultado) {
    if (raiz->esq != NO_VAZIO) {
        raiz->esq = _pega_min_rec(raiz->esq, resultado);
        return raiz->esq;
    }

    No prox = raiz->dir;

    *resultado = raiz->dado;

    free(raiz);

    return prox;
}

Dado pega_minimo(Treap treap) {
    if (treap->raiz == NO_VAZIO) {
        return NULL;
    }

    Dado resultado;
    treap->raiz = _pega_min_rec(treap->raiz, &resultado);

    return resultado;
}