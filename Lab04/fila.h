#ifndef _FILA_H
#define _FILA_H

#include <stdlib.h>

typedef struct _no *No;
typedef struct _fila *Fila;

Fila constroi_fila(void);
void destroi_fila(Fila fila, void (*desaloca)(const void *dado));

void enfileirar(Fila fila, const void *dado);
void *desenfileirar(Fila fila);

unsigned tamanho_fila(Fila fila);

#endif