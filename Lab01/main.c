#include "lab01.h"

int main()
{
	int np, na;
	Professor professores[100];
	Aluno alunos[200];

	scanf("%d", &np);

	ler_professores(professores, np);

	scanf("%d", &na);

	ler_alunos(alunos, na);

	aplicar_aumento(professores, np, alunos, na);

	imprimir_professores(professores, np);

	return 0;
}