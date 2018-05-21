#include "arv_gen.h"

#include <stdio.h>
#include <stdlib.h>
#include "arv_binaria.h"
#include <string.h>

#define qtd_folhas(h) \
    ((1 << (h)) - 1)

static int _stringcmp(Item a, Item b) {
    return strncmp(a, b, STR_MAX);
}

Genealogia constroi_gen(char **lista, unsigned altura) {
    struct _arv_g *nova = malloc(sizeof(struct _arv_g));
    unsigned ancestrais = qtd_folhas(altura);
    nova->arvore = constroi_arvore((Item *) lista, ancestrais, POSORDEM, _stringcmp);
    nova->lista = lista;
    nova->altura = altura;
    return nova;
}

Genealogia constroi_gen_de_arquivo(FILE *arquivo, unsigned altura) {
    unsigned i, ancestrais;
    char **lista;
    
    ancestrais = qtd_folhas(altura);

    lista = malloc(ancestrais * sizeof(char *));
    lista[0] = malloc(ancestrais * (STR_MAX+1) * sizeof(char));

    for (i = 0; i < ancestrais; i++) {
        lista[i] = lista[0] + i * ((STR_MAX+1) * sizeof(char));
        fscanf(arquivo, "%s", lista[i]);
    }

    return constroi_gen(lista, altura);
}

void destroi_gen(Genealogia gen) {
    destroi_arvore(gen->arvore);
    free(gen->lista[0]);
    free(gen->lista);
    free(gen);
}

#define escreve_texto(texto, nome, altura) \
    texto += sprintf(texto, "%s %u", nome, altura) * sizeof(char)

#define espaco(texto) \
    texto += sprintf(texto, " ") * sizeof(char)

#define fim_de_linha(texto) \
    texto += sprintf(texto, "\n") * sizeof(char)

static char *_rec_compara_gen(char *texto, No raiz, Arvore arv) {
    int altura;

    if (raiz == NULL) {
        return texto;
    }

    altura = buscar_item(arv, raiz->dado);

    if (altura >= 0) {
        escreve_texto(texto, (char *) raiz->dado, altura);
        espaco(texto);
    }

    texto = _rec_compara_gen(texto, raiz->esquerda, arv);
    texto = _rec_compara_gen(texto, raiz->direita, arv);

    return texto;
}

char *compara_genealogias(Genealogia gen1, Genealogia gen2) {
    unsigned ancestrais_gen1 = qtd_folhas(gen1->altura);
    unsigned ancestrais_gen2 = qtd_folhas(gen2->altura);
    unsigned capacidade_texto = (ancestrais_gen1 + ancestrais_gen2) * (STR_MAX + 4);

    char *resultado = calloc(capacidade_texto, sizeof(char));

    char *str_ptr = _rec_compara_gen(resultado, gen2->arvore->raiz, gen1->arvore);
    fim_de_linha(str_ptr);
    
    str_ptr = _rec_compara_gen(str_ptr, gen1->arvore->raiz, gen2->arvore);
    fim_de_linha(str_ptr);

    return resultado;
}
