#include "arv_gen.h"

#include <stdio.h>
#include <stdlib.h>
#include "arv_binaria.h"
#include <string.h>

#define qtd_folhas(h) \
    ((1 << (h)) - 1)

#define folha(k, h, n) \
    (((n-1)*k)/h - (h-1))

#define formato_leitura(qtd) "%" #qtd "s"

struct _arv_g {
    Arvore arvore;
    unsigned altura;
    String *lista;
};

static int _stringcmp(Item a, Item b) {
    return strncmp(a, b, STR_MAX);
}

static Genealogia _gen_init(String *lista, unsigned altura) {
    struct _arv_g *nova = malloc(sizeof(struct _arv_g));
    nova->arvore = constroi_arvore(NULL, _stringcmp);
    nova->lista = lista;
    nova->altura = altura;
    return nova;
}

Genealogia constroi_gen_de_lista(String *lista, unsigned altura) {
    Genealogia gen = _gen_init(lista, altura);

    unsigned ancestrais = qtd_folhas(altura);
    for (unsigned h = 1; h <= altura; h++) {
        for (unsigned k = 1; k <= h; k++) {
            inserir_item(gen->arvore, (Item) lista[folha(k, h, ancestrais)]);
        }
    }
    return gen;
}

Genealogia constroi_gen_de_arquivo(FILE *arquivo, unsigned altura) {
    unsigned ancestrais = qtd_folhas(altura);

    fprintf(stderr, "%u\n", ancestrais);

    String *lista = malloc(ancestrais * sizeof(String));

    for (unsigned i = 0; i < ancestrais; i++) {
        fscanf(arquivo, "%s", lista[i]);
    }

    return constroi_gen_de_lista(lista, altura);
}

void destroi_gen(Genealogia gen) {
    destroi_arvore(gen->arvore);
    free(gen->lista);
    free((void *) gen);
}

void compara_genealogias(Genealogia gen1, Genealogia gen2) {
    unsigned ancestrais_gen1 = qtd_folhas(gen1->altura);
    for (unsigned i = 0; i < ancestrais_gen1; i++) {
        if (buscar_item(gen2->arvore, (Item) gen1->lista[i]) >= 0) {
            int altura = buscar_item(gen1->arvore, (Item) gen1->lista[i]);
            printf("%s %d ", gen1->lista[i], altura);
        }
    }
    printf("\n");
    
    unsigned ancestrais_gen2 = qtd_folhas(gen2->altura);
    for (unsigned i = 0; i < ancestrais_gen2; i++) {
        if (buscar_item(gen1->arvore, (Item) gen2->lista[i]) >= 0) {
            int altura = buscar_item(gen2->arvore, (Item) gen2->lista[i]);
            printf("%s %d ", gen2->lista[i], altura);
        }
    }
    printf("\n");
}
