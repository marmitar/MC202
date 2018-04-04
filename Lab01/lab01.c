/* RA 187679
 * 2018s1 MC202 A
 * Lab 01
 */

#include <string.h>
#include "professor.h"
#include "aluno.h"

/* Lê cada um dos 'n' professores pela entrada padrão (memória alocada previamente).
 */
void ler_professores(Professor *professores, int n) {
    int i; for (i = 0; i < n; i++) {
        ler_professor(&(professores[i]));
    }
}

/* Similar a função acima, mas para alunos.
 */
void ler_alunos(Aluno *alunos, int n) {
    int i; for (i = 0; i < n; i++) {
        ler_aluno(&(alunos[i]));
    }
}

/* Aplica os respectivos aumentos salariais usando os dados já registrados.
 */
void aplicar_aumento(Professor *professores, int np, Aluno *alunos, int na) {

    /* para cada professor 'i' */
    int i; for (i = 0; i < np; i++) {
        
        /* encontra a menor média */
        double media_min = 11.0;
        /* dentre todos os alunos 'j' */
        int j; for (j = 0; j < na; j++) {
            int esta_matriculado = 0;
            /* em que suas disciplinas matriculadas 'k' */
            int k; for (k = 0; !esta_matriculado && k < alunos[j].num_disciplinas; k++) {
                /* contenha a displina do professor 'i' */
                if (strncmp(alunos[j].disciplinas[k], professores[i].disciplina, MAX_NOME_DISCIPLINA) == 0) {
                    
                    /* atualiza a menor até então */
                    if (media_min > alunos[j].media) {
                        media_min = alunos[j].media;
                    }

                    /* registra que o aluno está matriculado na disciplina para encerrar o laço */
                    esta_matriculado = 1;
                }
            }
        }

        /* erro */
        if (media_min > 10.0) {
            return;
        
        /* aplicar o respectivo aumento */
        } else if (media_min == 10.0) {
            aumento(&(professores[i]), .15);
        } else if (media_min >= 9.0) {
            aumento(&(professores[i]), .10);
        } else if (media_min >= 8.5) {
            aumento(&(professores[i]), .05);
        }
    }
}

/* Imprime cada professor e seu respectivo salário na saída padrão.
 */
void imprimir_professores(Professor *professores, int np) {
    int i; for (i = 0; i < np; i++) {
        printf("%s %s %.02f\n", professores[i].nome, professores[i].sobrenome, professores[i].salario);
    }
}