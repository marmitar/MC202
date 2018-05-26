/* Tiago de Paula Alves - 187679 - MC202 A - Lab. 8 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "abb.h"

#define STR_MAX 80
typedef char *String;
typedef char StringMax[STR_MAX + 1];

/* construtor de nome de livro */
String novo_livro(String nome) {
    String novo = malloc((STR_MAX + 1) * sizeof(char));
    strcpy(novo, nome);

    return novo;
}
/* comparador de nomes de livros, que são apenas strings */
Comparador cmp_livro = (Comparador) strcmp;
/* impressor de nome de livro */
void imprime_livro(Elemento livro) {
    printf("%s\n", (String) livro);
}
/* destrutor de nome de livro, que são apenas ponteiros alocados */
Destrutor libera_livro = (Destrutor) free;

/* imprime sugestões com base em um prefixo */
void imprime_sugestoes(ABB estoque, String prefixo) {
    StringMax final_intervalo;
    strcpy(final_intervalo, prefixo);
    final_intervalo[strlen(prefixo) - 1] += 1;

    printf("Sugestoes para prefixo \"%s\":\n", prefixo);
    imprime_intervalo(estoque, prefixo, final_intervalo);
}

int main(void) {
    ABB estoque = constroi_abb(cmp_livro, imprime_livro, libera_livro);

    char op; StringMax arg;
    bool fim = false;
    while (!fim && scanf(" %c %[^\n]", &op, arg) == 2) {
        switch (op) {
            case '+':
                insere_elemento(estoque, novo_livro(arg));
            break;
            case '-':
                remove_elemento(estoque, arg);
            break;
            case '$':
                imprime_sugestoes(estoque, arg);
            break;
            case '#':
                fim = true;
        }
    }

    destroi_abb(estoque);
    return 0;
}