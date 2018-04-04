/* RA 187679
 * 2018s1 MC202 A
 * Lab 01
 */

#include <stdio.h>
#include "professor.h"

/* Lê um professor da entrada padrão (formatado corretamente).
 */
void ler_professor(Professor *professor) {
    scanf("%s %s %lf %s", professor->nome, professor->sobrenome, &(professor->salario), professor->disciplina);
}

/* Aplica um aumento percentual no salário do professor.
 */
void aumento(Professor *professor, double porcentagem) {
    professor->salario += professor->salario * porcentagem;
}
