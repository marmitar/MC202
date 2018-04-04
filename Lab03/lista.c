/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 03
 */

/* Biblioteca de lista ligada ordenada genérica. */
#include "lista.h"

#include <stdlib.h>
#include <stdbool.h>

/* Todas as funções de macro são usadas somente para coisas simples e têm escopo limitado */

/* Testa se um nó é vazio ou não */
#define eh_vazio(ptr) \
    (ptr == NULL)

/* Testes do resultado da função de comparação */
#define primeiro_arg_maior(resultado_comparacao) \
    (resultado_comparacao > 0)
#define segundo_arg_maior(resultado_comparacao) \
    (resultado_comparacao < 0)
#define args_iguais(resultado_comparacao) \
    (resultado_comparacao == 0)

/* Aloca memória para um novo nó. Escopo local. */
static No constroi_no(const void *dado, No prox) {
    No novo = malloc(sizeof(struct _no));
    novo->dado = dado;
    novo->prox = prox;
    return novo;
}

/* Construtor e destrutor da lista ligada. */
Lista constroi_lista(void) {
    return (Lista) calloc(1, sizeof(struct _lst));
}
void destroi_lista(Lista lista, void (*desaloca)(const void *dado)) {
    /* apontador que percorre a lista */
    No ptr = lista->cab;

    /* para cada nó */
    while (! eh_vazio(ptr)) {
        No prox = ptr->prox;
        desaloca(ptr->dado); /* libera memória do tipo usado */
        free(ptr); /* libera o nó */
        ptr = prox;
    }

    free(lista); /* por fim, libera a lista */
}

/* Funçoes de manuseamento de dados da lista ligada ordenada. */
void insere_dado(Lista lista, const void *dado, int (*compara)(const void *primeiro, const void *segundo)) {
    /* ponteiro para percorrer a lista */
    No ptr = lista->cab;

    /* lista vazia ou primeiro elemento (e, logo, todo o resto) maior que o elemento a ser gerado */
    if (eh_vazio(ptr) || primeiro_arg_maior(compara(ptr->dado, dado))) {
        lista->cab = constroi_no(dado, ptr);
        return;
    }

    /* acha o último nó ou o nó em que o seguinte é maior que o dado a ser inserido */
    while (! eh_vazio(ptr->prox) && segundo_arg_maior(compara(ptr->prox->dado, dado))) {
        ptr = ptr->prox;
    }

    /* insere o nó naquela posição */
    ptr->prox = constroi_no(dado, ptr->prox);
}
bool remove_dado(Lista lista, const void *dado, int (*compara)(const void *primeiro, const void *segundo), void (*desaloca)(const void *dado)) {
    No ptr = lista->cab; /* ponteiro */

    /* lista vazia */
    if (eh_vazio(ptr)) {
        /* sem dados para remover */
        return false;
    }

    /* remove dado do começo da lista */
    if (args_iguais(compara(ptr->dado, dado))) {
        lista->cab = lista->cab->prox;
        desaloca(ptr->dado);
        free(ptr);
        return true;
    }

    /* encontra último nó ou último nó menor que o nó deve ser removido */
    while (! eh_vazio(ptr->prox) && segundo_arg_maior(compara(ptr->prox->dado, dado))) {
        ptr = ptr->prox;
    }

    /* se é o último nó, a lista não tem o dado para ser removido */
    if (eh_vazio(ptr->prox)) {
        return false;
    }

    /* dado encontrado */
    if (args_iguais(compara(ptr->prox->dado, dado))) {
        No prox = ptr->prox->prox;
        desaloca(ptr->prox->dado);
        free(ptr->prox);
        ptr->prox = prox;
        return true;
    }

    /* lista não tem nenhum dado para ser removido */
    return false;
}

/* evita o "vazamento" das funções macro */
#undef eh_vazio
#undef primeiro_arg_maior
#undef segundo_arg_maior
#undef args_iguais