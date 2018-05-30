#include <stdio.h>
#include "arv_binaria.h"
#include <stdlib.h>
#include <string.h>

/* tipos de string */
typedef char *String;
#define STR_MAX 20
typedef char StringMax[STR_MAX + 1];

/* comparador de nomes na genealogia */
Comparador nome_cmp = (Comparador) strcmp;

/* Número de elementos para uma árvore com a dada altura. */
#define num_elementos(altura) \
    ((1 << (altura)) - 1)

/* Lê 'n' nomes (com até STR_MAX caracteres) da entrada padrão. */
StringMax *le_nomes(int n) {
    StringMax *nomes = malloc(n * sizeof(StringMax));
    
    int i; for (i = 0; i < n; i++) {
        scanf(" %s", nomes[i]);
    }

    return nomes;
}

/* Monta árvore genealógica com lista de nomes. */
Arvore monta_gen(StringMax *nomes, int tam) {
    Arvore gen;

    /* vetor com apenas os ponteiros para as strings */
    /* serve para usar na biblioteca, que recebe apenas lista de ponteiros */
    String *p_nomes = malloc(tam * sizeof(String));
    int i; for (i = 0; i < tam; i++) {
        p_nomes[i] = nomes[i];
    }

    gen = constroi_de_lista((Dado *) p_nomes, tam, POSORDEM);

    /* não precisa mais do vetor de ponteiros */
    free(p_nomes);

    return gen;
}

void imprime_intersec(Elemento *nomes) {
    int i; for (i = 0; nomes[i] != NULL; i++) {
        printf("%s %lu ", (String) nomes[i]->dado, nomes[i]->altura);
    }
    printf("\n");
}

int main(void) {
    Arvore g1, g2;
    StringMax *l1, *l2;
    Elemento *intersec_1, *intersec_2;
    int H1, H2, N1, N2;

    /* lê as alturas */
    scanf("%d %d", &H1, &H2);

    N1 = num_elementos(H1);
    N2 = num_elementos(H2);

    l1 = le_nomes(N1);
    l2 = le_nomes(N2);

    g1 = monta_gen(l1, N1);
    g2 = monta_gen(l2, N2);

    /* encontra a intersecção de g1 com g2, com base em g1 */
    intersec_1 = interseccao(g1, g2, nome_cmp, PREORDEM);
    /* e vice versa */
    intersec_2 = interseccao(g2, g1, nome_cmp, PREORDEM);

    imprime_intersec(intersec_1);
    imprime_intersec(intersec_2);

    destroi_lista(intersec_1);
    destroi_lista(intersec_2);

    destroi_arvore(g1);
    destroi_arvore(g2);

    free(l1);
    free(l2);

    return 0;
}