#include <stdio.h>
#include <stdbool.h>
#include "conjhash.h"
#include <stdlib.h>
#include <string.h>

#define STR_MAX 50
typedef char *String;
typedef char StringMax[STR_MAX + 1];

int main(void) {
    ConjHash conj_chaves;
    StringMax chave; bool fim;

    conj_chaves = novo_conjhash(free, (Comparador) strcmp);


    fim = false;
    while (!fim && scanf(" %[^\n]", chave) > 0) {
        /* caso de parada */
        if (strcmp(chave, "#") == 0) {
            fim = true;

        } else {
            bool repetido;
            /* tamanho (em bytes) da string */
            size_t tam = strlen(chave) * sizeof(char);

            /* copia a string para uma nova memória */
            String nova = malloc(tam + sizeof(char));
            memcpy(nova, chave, tam + sizeof(char));

            /* tenta inserir, testando a repetição */
            repetido = !insere_elem(conj_chaves, nova, tam);
            if (repetido) {
                free(nova);
            }

            printf("%d %s\n", repetido, chave);
        }
    }

    /* libera memória */
    del_conjhash(conj_chaves);
    return 0;
}