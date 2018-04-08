#ifndef _FILA_H
#define _FILA_H

#include <stdlib.h>
#include <stdarg.h>

typedef struct _no *No;
typedef struct _fila *Fila;

Fila constroi_fila(void);
void destroi_fila(Fila fila, void (*desaloca)(const void *dado));

void enfileirar(Fila fila, const void *dado);
void *desenfileirar(Fila fila);

void atualizar_dados(Fila fila, void (atualiza)(const void *dado, va_list args), ...);

unsigned tamanho_fila(Fila fila);

#endif