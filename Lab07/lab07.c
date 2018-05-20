#include <stdio.h>
#include "arv_gen.h"

int main(void) {
    int altura_gen1, altura_gen2;
    scanf("%d %d", &altura_gen1, &altura_gen2);

    Genealogia gen1 = constroi_gen_de_arquivo(stdin, altura_gen1);
    Genealogia gen2 = constroi_gen_de_arquivo(stdin, altura_gen2);

    compara_genealogias(gen1, gen2);

    destroi_gen(gen1);
    destroi_gen(gen2);

    return 0;
}