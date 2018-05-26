/* Tiago de Paula Alves - 187679 - MC202 A - Lab. 8 */

#include "abb.h"

/******************************************************
 * IMPLETAÇÃO DA BIBLITECA DE ÁRVORE BINÁRIA DE BUSCA *
 ******************************************************/

#include <stdlib.h>

typedef struct _no {
    Elemento elemento;
    struct _no *pai;
    struct _no *esquerdo;
    struct _no *direito;
} *No;
#define NO_VAZIO ((No) NULL)

struct _abb {
    No raiz;
    Comparador comparador;
    Impressor impressor;
    Destrutor destrutor;
};


/* * * * * * *
 * Construtor *
  * * * * * * */
ABB constroi_abb(Comparador comparador, Impressor impressor, Destrutor destrutor) {
    ABB nova = malloc(sizeof(struct _abb));

    nova->raiz = NO_VAZIO;
    nova->comparador = comparador;
    nova->impressor = impressor;
    nova->destrutor = destrutor;
    return nova;
}


/* * * * * * *
 * Destrutor *
 * * * * * * */
/* destroi um nó e seus filhos recursivamente */
static void _destroi_no_rec(No no, Destrutor destrutor) {
    if (no == NO_VAZIO) {
        return;
    }

    _destroi_no_rec(no->esquerdo, destrutor);
    _destroi_no_rec(no->direito, destrutor);

    destrutor(no->elemento);
    free(no);
}
/* destroi a árvore inteira */
void destroi_abb(ABB arvore) {
    _destroi_no_rec(arvore->raiz, arvore->destrutor);
    free(arvore);
}






/* * * * * *
 * Inserção *
  * * * * * */
static void _insere_no(No raiz, No novo, Comparador comparador) {
    /* busca iterativa do pai do novo nó */
    No pai, proximo_pai;
    for (proximo_pai = raiz; proximo_pai != NO_VAZIO;) {
        pai = proximo_pai;

        if (comparador(pai->elemento, novo->elemento) > 0) {
            proximo_pai = pai->esquerdo;
        } else {
            proximo_pai = pai->direito;
        }
    }
    
    /* insere o elemento */
    novo->pai = pai;
    if (comparador(pai->elemento, novo->elemento) > 0) {
        pai->esquerdo = novo;
    } else {
        pai->direito = novo;
    }
}
void insere_elemento(ABB arvore, Elemento elemento) {
    No novo = malloc(sizeof(struct _no));
    novo->elemento = elemento;
    novo->pai = NO_VAZIO;
    novo->esquerdo = NO_VAZIO;
    novo->direito = NO_VAZIO;

    if (arvore->raiz == NO_VAZIO) {
        arvore->raiz = novo;
    } else {
        _insere_no(arvore->raiz, novo, arvore->comparador);
    }
}

/* * * * * *
 * Remoção *
 * * * * * */
/* encontra o nó que tem o elemento */
static No _busca_elemento(ABB arvore, Elemento elemento) {
    No no, proximo;
    for (no = arvore->raiz; no != NO_VAZIO; no = proximo) {
        int comparacao = arvore->comparador(no->elemento, elemento);
        if (comparacao > 0) {
            proximo = no->esquerdo;
        } else if (comparacao < 0) {
            proximo = no->direito;
        } else {
            return no;
        }
    }

    return NO_VAZIO;
}
/* menor elemento a partir de um nó */
static No _minimo(No raiz) {
    No minimo = raiz;
    while (minimo->esquerdo != NO_VAZIO) {
        minimo = minimo->esquerdo;
    }
    return minimo;
}
/* remove um nó que tenha no máximo um filho */
static void _remove_no_linear(ABB arvore, No no) {
    No proximo = (no->direito != NO_VAZIO? no->direito : no->esquerdo);

    /* remove as referências do pai */
    if (no->pai == NO_VAZIO) {
        arvore->raiz = proximo;

    } else {
        if (no->pai->esquerdo == no) {
            no->pai->esquerdo = proximo;
        } else {
            no->pai->direito = proximo;
        }
    }
    /* e do filho */
    if (proximo != NO_VAZIO) {
        proximo->pai = no->pai;
    }

    arvore->destrutor(no->elemento);
    free(no);
}
/* busca e remove elemento */
void remove_elemento(ABB arvore, Elemento elemento) {
    No no, sucessor;
    Elemento salva_troca;

    no = _busca_elemento(arvore, elemento);

    /* se não tem sucessor, remove o nó */
    if (no->direito == NO_VAZIO) {
        _remove_no_linear(arvore, no);
        return;
    }

    /* se tem, troca com o sucessor e remove o nó trocado */
    sucessor = _minimo(no->direito);

    salva_troca = no->elemento;
    no->elemento = sucessor->elemento;
    sucessor->elemento = salva_troca;
    
    _remove_no_linear(arvore, sucessor);
}

/* * * * * * *
 * Impressão *
 * * * * * * */
static void _imprime_intervalo_rec(ABB arv, No no, Elemento ini, Elemento fim) {
    if (no == NO_VAZIO) {
            return;
    }

    /* imprime sub-árvore esquerda antes */
    _imprime_intervalo_rec(arv, no->esquerdo, ini, fim);
    /* imprime o nó se estiver no intervalo */
    if (arv->comparador(no->elemento, ini) >= 0 && arv->comparador(no->elemento, fim) < 0) {
        arv->impressor(no->elemento);
    }
    /* sub-árvore direita depois */
    _imprime_intervalo_rec(arv, no->direito, ini, fim);
}
void imprime_intervalo(ABB arvore, Elemento inicio, Elemento fim) {
    _imprime_intervalo_rec(arvore, arvore->raiz, inicio, fim);
}

#undef NO_VAZIO