/*  Tiago de Paula Alves - 187679
 *  MC202 A - 2018s1
 */
#ifndef __DRONE_H__
#define __DRONE_H__

#include <stddef.h>

/* * * * * * * * * * * * * * * * *
 * BIBLIOTECA DO SISTEMA DO DRONE *
  * * * * * * * * * * * * * * * * */

/* tipo de mapa */
typedef int **matriz_t;
typedef struct _mapa_ {
    matriz_t matriz;
    size_t lin, col;
} *Mapa;
Mapa aloca_mapa(size_t lin, size_t col);
void desaloca_mapa(Mapa);

typedef struct _drone_ *Drone;

/* construtor e destrutor */
Drone novo_drone(unsigned altura_max, size_t x_inicial, size_t y_inicial);
void destroi_drone(Drone);

/* drone faz o reconhecimento do mapa */
void reconhece_mapa(Drone, Mapa);

/* drone busca um caminho até a base, com -1 onde não o drone não passou */
Mapa analisa_caminho(Drone, size_t x_final, size_t y_final);

#endif/*__DRONE_H__*/