#include "conjhash.h"

/* arquivos auxiliares com as funções restantes */
#include "conjhash_listalig.h"
#include "conjhash_funchash.h"

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

struct _conj_hash {
    No *tabela;
    chave_t semente;

    Destrutor destr;
    Comparador cmp;
};

/* gera um semente aleatória para hashing */
static chave_t gen_semente(void) {
    static bool semeado = false;
    if (! semeado) {
        srand(time(NULL));
        semeado = true;
    }

    return (chave_t) rand();
}

/* * * * * * * * * * * * *
 * Construtor e Destrutor *
  * * * * * * * * * * * * */
ConjHash novo_conjhash(Destrutor destr, Comparador cmp) {
    ConjHash novo = malloc(sizeof(struct _conj_hash));
    novo->destr = destr;
    novo->cmp = cmp;
    /* semente do Conjunto */
    novo->semente = gen_semente();
    /* vetor de nós vazios */
    novo->tabela = calloc(HASH_MAX + 1, sizeof(No));

    return novo;
}
void del_conjhash(ConjHash conj) {
    size_t i; for (i = 0; i <= HASH_MAX; i++) {
        destroi_lista(conj->tabela[i], conj->destr);
    }
    free(conj->tabela);
    free(conj);
}

/* * * * * *
 * Inserção *
  * * * * * */
bool insere_elem(ConjHash conj, dado_t dado, size_t tam) {
    /* endereço do dado, usando a semente do Conjunto */
    chave_t chave = hash(conj->semente, dado, tam);

    /* se está vazia a posição, insere lá */
    if (conj->tabela[chave] == NO_VAZIO) {
        conj->tabela[chave] = novo_no(dado);
        return true;
    }

    /* se tem colisão, insere na lista */
    return insere_dado(conj->tabela[chave], dado, conj->cmp);
}