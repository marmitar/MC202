#include "arv_binaria.h"

/********************
 * ÁRVORES BINÁRIAS *
 ********************/

#include <stdlib.h>
#include <stdbool.h>


typedef struct _no {
    Dado dado;
    struct _no *esq, *dir;
} *No;
#define NO_VAZIO ((No) NULL)

struct _arv_bin {
    No raiz;
    size_t tam;
};

/* * * * * * *
 * Construtor *
  * * * * * * */

/*  Auxiliar do construtor. Constroi a raiz da nova árvore recursivamente
 * em um intervalo inclusivo de 'ini' à 'fim' do vetor.
 */
static No _constroi_rec(Dado *lista, long ini, long fim, Ordem ordem) {
    No novo; long mid;
    if (ini > fim) {
        return NO_VAZIO;
    }

    novo = malloc(sizeof(struct _no));
    mid = (ini + fim)/2; /* meio do intervalo */

    switch(ordem) {
        case PREORDEM:
            novo->dado = lista[ini];
            novo->esq = _constroi_rec(lista, ini+1, mid, ordem);
            novo->dir = _constroi_rec(lista, mid+1, fim, ordem);
        break;
        case INORDEM:
            novo->esq = _constroi_rec(lista, ini, mid-1, ordem);
            novo->dado = lista[mid];
            novo->dir = _constroi_rec(lista, mid+1, fim, ordem);
        break;
        case POSORDEM:
            novo->esq = _constroi_rec(lista, ini, mid-1, ordem);
            novo->dir = _constroi_rec(lista, mid, fim-1, ordem);
            novo->dado = lista[fim];
        break;
    }

    return novo;
}

Arvore constroi_de_lista(Dado *lista, size_t tamanho, Ordem ordem) {
    Arvore nova = malloc(sizeof(struct _arv_bin));
    nova->raiz = _constroi_rec(lista, 0, tamanho-1, ordem);
    nova->tam = tamanho;

    return nova;
}


/* * * * * * *
 * Destrutor *
 * * * * * * */

static void _destroi_no_rec(No raiz) {
    if (raiz == NO_VAZIO) {
        return;
    }

    /* destrói sub-arvores */
    _destroi_no_rec(raiz->esq);
    _destroi_no_rec(raiz->dir);
    /* e, então, o pŕoprio nó */
    free(raiz);
}

void destroi_arvore(Arvore arvore) {
    _destroi_no_rec(arvore->raiz);
    free(arvore);
}


/* * * * * * * * * * * *
 * Teste de continência *
  * * * * * * * * * * * */

/* busca um nó com aquele dado */
static No _busca_dado_rec(No raiz, Dado dado, Comparador cmp) {
    No resultado;
    if (raiz == NO_VAZIO) {
        return NO_VAZIO;
    }

    /* se achar o elemento, retorna seu nó */
    if (cmp(raiz->dado, dado) == 0) {
        return raiz;
    }

    /* senão, tenta busca na sub-arvore esquerda */
    resultado = _busca_dado_rec(raiz->esq, dado, cmp);
    if (resultado != NO_VAZIO) {
        return resultado;
    }

    /* depois na direita */
    resultado = _busca_dado_rec(raiz->dir, dado, cmp);
    if (resultado != NO_VAZIO) {
        return resultado;
    }

    /* não achou */
    return NO_VAZIO;
}

bool tem_elemento(Arvore arv, Dado elemento, Comparador cmp) {
    return _busca_dado_rec(arv->raiz, elemento, cmp) != NO_VAZIO;
}


/* * * * * * * *
 * Intersecção *
 * * * * * * * */

/* novo elemento de intersecção */
static Elemento novo_elem(Dado dado, size_t altura) {
    Elemento novo = malloc(sizeof(struct _elem));
    novo->dado = dado;
    novo->altura = altura;

    return novo;
}
void destroi_lista(Elemento *lista) {
    size_t i; for (i = 0; lista[i] != NULL; i++) {
        free(lista[i]);
    }
    free(lista);
}

/* encontra a intersecção da sub-árvore no nó 'raiz' com a outra árvore */
/* recebe o tamanho da lista de interseção e retorna o tamanho atualizado */
static size_t _intersec_rec(Elemento *lista, size_t tam, size_t altura, No raiz, Ordem ordem, Arvore outra, Comparador cmp) {
    if (raiz == NO_VAZIO) {
        return tam; /* não muou o tamanho */
    }

    switch(ordem) {
        case PREORDEM:
            if (tem_elemento(outra, raiz->dado, cmp)) {
                lista[tam++] = novo_elem(raiz->dado, altura);
            }
            tam = _intersec_rec(lista, tam, altura+1, raiz->esq, ordem, outra, cmp);
            tam = _intersec_rec(lista, tam, altura+1, raiz->dir, ordem, outra, cmp);
        break;
        case INORDEM:
            tam = _intersec_rec(lista, tam, altura+1, raiz->esq, ordem, outra, cmp);
            if (tem_elemento(outra, raiz->dado, cmp)) {
                lista[tam++] = novo_elem(raiz->dado, altura);
            }
            tam = _intersec_rec(lista, tam, altura+1, raiz->dir, ordem, outra, cmp);
        break;
        case POSORDEM:
            tam = _intersec_rec(lista, tam, altura+1, raiz->esq, ordem, outra, cmp);
            tam = _intersec_rec(lista, tam, altura+1, raiz->dir, ordem, outra, cmp);
            if (tem_elemento(outra, raiz->dado, cmp)) {
                lista[tam++] = novo_elem(raiz->dado, altura);
            }
        break;
    }

    return tam;
}

Elemento *interseccao(Arvore a, Arvore b, Comparador cmp, Ordem ordem) {
    /* a interseção pode ter no máximo todos os elementos da árvore */
    /* mais um para o elemento vazio no final */
    Elemento *intersec = malloc((a->tam + 1) * sizeof(Elemento));

    /* monta a intersecção e encontra seu tamanho */
    size_t tamanho = _intersec_rec(intersec, 0, 0, a->raiz, ordem, b, cmp);

    /* marca o final da lista de intersecção */
    intersec[tamanho] = NULL;

    return intersec;
}

#undef NO_VAZIO