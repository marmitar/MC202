#ifndef ALUNO_H
#define ALUNO_H

#include <stdio.h>

#define MAX_NOME 11
#define MAX_DISCIPLINA 5
#define MAX_NOME_DISCIPLINA 6

typedef struct _aluno
{
	char nome[MAX_NOME];
	char sobrenome[MAX_NOME];
	double media;
	int num_disciplinas;
	char disciplinas[MAX_DISCIPLINA][MAX_NOME_DISCIPLINA];
} Aluno;

void ler_aluno(Aluno *aluno);

#endif
