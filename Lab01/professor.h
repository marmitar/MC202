#ifndef PROFESSOR_H
#define PROFESSOR_H

#include <stdio.h>

#define MAX_NOME 11
#define MAX_NOME_DISCIPLINA 6

typedef struct _professor
{
	char nome[MAX_NOME];
	char sobrenome[MAX_NOME];
	double salario;
	char disciplina[MAX_NOME_DISCIPLINA];
} Professor;

void ler_professor(Professor *professor);

void aumento(Professor *professor, double porcentagem);

#endif
