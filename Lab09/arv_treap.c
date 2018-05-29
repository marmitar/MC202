#include "arv_treap.h"

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

/*  bibliotecas em UNIX para usar ruído ambiental
   como gerador de números aleatórios */
#ifdef __unix__
#include <sys/syscall.h>
#ifdef SYS_getrandom
#include <sys/random.h>
#endif
#endif

typedef long unsigned Prio;

typedef struct _no {
    Dado dado;
    Prio pri; /* prioridade */
    struct _no *esq, *dir;
} *No;
#define NO_VAZIO ((No) NULL)

struct _treap {
    No raiz;
    Comparador cmp;
    Destrutor destr;
};

/* * * * * * *
 * Construtor *
  * * * * * * */

Treap constroi_treap(Comparador comparador, Destrutor destrutor) {
    Treap novo = malloc(sizeof(struct _treap));
    novo->raiz = NO_VAZIO;
    novo->cmp = comparador;
    novo->destr = destrutor;
    return novo;
}


/* * * * * * *
 * Destrutor *
 * * * * * * */

static void _destroi_no_rec(Destrutor destroi, No raiz) {
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


/* * * * * *
 * Inserção *
  * * * * * */

/* gerador de prioridade aletória */
/* quão mais aleatório for, normalmente mais otimizada a estrutura */
static Prio _gerar_prio(void) {
    static bool semeado = false;

#ifdef SYS_getrandom
    /* se possível, usa ruídos ambientais, que é a opção mais aleatória */
    Prio nova;
    if (getrandom(&nova, sizeof(Prio), GRND_RANDOM | GRND_NONBLOCK) == sizeof(Prio)) {
        return nova;
    } else if (getrandom(&nova, sizeof(Prio), GRND_NONBLOCK) == sizeof(Prio)) {
        return nova;
    }
#endif

    /* semea apenas uma vez, usando varíavel estática para marcar */
    /* semear toda vez faz a treap rodar perceptivelmente mais lenta */
    if (! semeado) {
        srand(time(NULL));
        semeado = true;
    }

    return rand();
}

/* rotações */
static No _rotaciona_p_esq(No x) {
    No y = x->dir;

    x->dir = y->esq;
    y->esq = x;

    return y;
}
static No _rotaciona_p_dir(No y) {
    No x = y->esq;

    y->esq = x->dir;
    x->dir = y;

    return x;
}

static No _insere_no_rec(Comparador cmp, No raiz, No novo) {
    if (raiz == NO_VAZIO) {
        /* insere na folha */
        return novo;
    }
    
    /* se o novo dado é maior que a raiz */
    if (cmp(novo->dado, raiz->dado) > 0) {
        /* insere na árvore */
        raiz->dir = _insere_no_rec(cmp, raiz->dir, novo);
        /* e corrige a heap */
        if (raiz->dir->pri > raiz->pri) {
            raiz = _rotaciona_p_esq(raiz);
        }

    /* do mesmo modo, se for menor */
    } else {
        raiz->esq = _insere_no_rec(cmp, raiz->esq, novo);
        if (raiz->esq->pri > raiz->pri) {
            raiz = _rotaciona_p_dir(raiz);
        }
    }

    return raiz;
}

void insere_dado(Treap treap, Dado dado) {
    No novo = malloc(sizeof(struct _no));
    novo->dado = dado;
    novo->pri = _gerar_prio();
    novo->esq = novo->dir = NO_VAZIO;

    treap->raiz =_insere_no_rec(treap->cmp, treap->raiz, novo);
}


/* static No _remove_dado_rec(Comparador cmp, No raiz, Dado dado) {

}

void remove_dado(Treap treap, Dado dado) {
} */

/* * * * *
 * Mínimo *
  * * * * */

static No _pega_min_rec(No raiz, Dado *resultado) {
    No prox;

    /* o mínimo é apenas o nó mais a esquerda possível */
    if (raiz->esq != NO_VAZIO) {
        raiz->esq = _pega_min_rec(raiz->esq, resultado);
        return raiz;
    }

    *resultado = raiz->dado;

    prox = raiz->dir;
    free(raiz);

    return prox;
}

Dado pega_minimo(Treap treap) {
    Dado minimo;
    if (treap->raiz == NO_VAZIO) {
        return NULL;
    }

    treap->raiz = _pega_min_rec(treap->raiz, &minimo);

    return minimo;
}