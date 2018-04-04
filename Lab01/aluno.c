/* RA 187679
 * 2018s1 MC202 A
 * Lab 01
 */

#include <stdio.h>
#include "aluno.h"

/* Lê um aluno da entrada padrão (formatado corretamente).
 */
void ler_aluno(Aluno *aluno) {
    int i;

    /* lê os dados inicias do aluno */
    scanf("%s %s %lf %d", aluno->nome, aluno->sobrenome, &(aluno->media), &(aluno->num_disciplinas));

    /* lê cada uma das matérias que ele faz */
    for (i = 0; i < aluno->num_disciplinas; i++) {
        scanf("%s", aluno->disciplinas[i]);
    }
}