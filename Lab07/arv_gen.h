#ifndef _ARV_GEN_H
#define _ARV_GEN_H

#include <stdio.h>
#include "arv_binaria.h"

#define STR_MAX 20

typedef struct _arv_g {
    Arvore arvore;
    unsigned altura;
    char **lista;
} *Genealogia;

Genealogia constroi_gen(char **lista, unsigned altura);
Genealogia constroi_gen_de_arquivo(FILE *arquivo, unsigned altura);
void destroi_gen(Genealogia gen);

char *compara_genealogias(Genealogia, Genealogia);

#endif