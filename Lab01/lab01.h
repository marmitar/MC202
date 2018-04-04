#ifndef LAB01_H
#define LAB01_H

#include <string.h>
#include "professor.h"
#include "aluno.h"

void ler_professores(Professor *professores, int n);

void ler_alunos(Aluno *alunos, int n);

void aplicar_aumento(Professor *professores, int np, Aluno *alunos, int na);

void imprimir_professores(Professor *professores, int np);

#endif
