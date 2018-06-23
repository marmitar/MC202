#ifndef __DRONE_H__
#define __DRONE_H__

#include <stddef.h>

typedef int **matriz_t;
typedef struct _mapa_ {
    matriz_t matriz;
    size_t lin, col;
} *Mapa;
Mapa aloca_mapa(size_t lin, size_t col);
void desaloca_mapa(Mapa);

typedef struct _drone_ *Drone;

Drone novo_drone(unsigned altura_max, size_t x_inicial, size_t y_inicial);
void destroi_drone(Drone);

void reconhece_mapa(Drone, Mapa);
Mapa analisa_caminho(Drone, size_t x_final, size_t y_final);

#endif/*__DRONE_H__*/