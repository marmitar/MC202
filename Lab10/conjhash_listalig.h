/**********************
 * CABEÇALHO AUXILIAR *
 **********************
 *    LISTA LIGADA    *
 **********************/

/* lista ligada simples, sem cabeça,
 * desordenada e  sem repetições
 */

#include <stdlib.h>
#include <stdbool.h>

typedef struct _no {
    struct _no *prox;
    dado_t dado;
} *No;
#define NO_VAZIO ((No) NULL)

/* * * * * * * * * * * * *
 * Construtor e Destrutor *
  * * * * * * * * * * * * */
static No novo_no(dado_t dado) {
    No novo = malloc(sizeof(struct _no));
    novo->prox = NO_VAZIO;
    novo->dado = dado;
    return novo;
}
static void destroi_lista(No raiz, Destrutor destr) {
    while (raiz != NO_VAZIO) {
        No aux = raiz;
        raiz = raiz->prox;
        destr(aux->dado);
        free(aux);
    }
}

/* * * * * *
 * Inserção *
  * * * * * */
static bool insere_dado(No raiz, dado_t dado, Comparador cmp) {
    /* procura pelas repetições na lista */
    No no = raiz; No ant = NO_VAZIO;
    while (no != NO_VAZIO) {
        if (cmp(no->dado, dado) == 0) {
            return false;
        }

        ant = no;
        no = no->prox;
    }

    /* isnere no fim */
    ant->prox = novo_no(dado);
    return true;
}