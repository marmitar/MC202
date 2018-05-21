#include <stdio.h>
#include <stdlib.h>
#include "arv_gen.h"

int main(void) {
    Genealogia gen1, gen2;
    char *impressao;

    int altura_gen1, altura_gen2;
    scanf("%d %d", &altura_gen1, &altura_gen2);

    gen1 = constroi_gen_de_arquivo(stdin, altura_gen1);
    gen2 = constroi_gen_de_arquivo(stdin, altura_gen2);

    impressao = compara_genealogias(gen1, gen2);
    printf("%s", impressao);
    free(impressao);

    destroi_gen(gen1);
    destroi_gen(gen2);

    return 0;
}