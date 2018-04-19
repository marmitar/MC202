#ifndef _PILHA_H
#define _PILHA_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct _pilha *Pilha;

Pilha constroi_pilha(void);
void destroi_pilha(Pilha pilha, void (*desaloca)(const void *dado));

bool empilhar(Pilha pilha, const void *dado);
void *desempilhar(Pilha pilha);

void *pegar_proximo(Pilha pilha);

#endif