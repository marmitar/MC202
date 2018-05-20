#ifndef _ARV_GEN_H
#define _ARV_GEN_H

#include <stdio.h>

#define STR_MAX 20
typedef char String[STR_MAX+1];

typedef const struct _arv_g *Genealogia;

Genealogia constroi_gen_de_lista(String *lista, unsigned altura);
Genealogia constroi_gen_de_arquivo(FILE *arquivo, unsigned altura);
void destroi_gen(Genealogia gen);

void compara_genealogias(Genealogia, Genealogia);

#endif