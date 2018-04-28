#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "pilha.h"

/*  Funções que ajudam a escrever o texto de saída. Foram feitas em macro, pois
 * são apenas wrappers de 'sprintf' e também por permitir a alteração de um
 * argumento sem a referência, caso o tamanho.
 */
#define escreve_desfile(texto, tamanho, capacidade, alegoria) \
    tamanho += sprintf(texto + tamanho * sizeof(char), \
        "Alegoria %u desfila.\n", alegoria)

#define escreve_entrada(texto, tamanho, capacidade, alegoria, rua) \
    tamanho += sprintf(texto + tamanho * sizeof(char), \
        "Alegoria %u entra na rua %u e aguarda.\n", alegoria, rua)

#define escreve_saida(texto, tamanho, capacidade, alegoria, rua) \
    tamanho += sprintf(texto + tamanho * sizeof(char), \
        "Alegoria %u sai da rua %u e desfila.\n", alegoria, rua)

/* Tenta encaixar uma alegoria em alguma paralela. */
unsigned encaixa_alegoria(Pilha *ruas, unsigned qtd_ruas, unsigned alegoria) {
    unsigned i; for (i = 0; i < qtd_ruas; i++) {
        unsigned *aleg_aguardando = ver_topo(ruas[i]);
        
        /* se não tiver nenhuma alegoria ou a alegoria que tem desfila depois */
        if (aleg_aguardando == NULL || (uintptr_t) aleg_aguardando > alegoria) {
            /* coloca a nova alegoria na perpendicular */
            empilhar(ruas[i], (void *)(uintptr_t) alegoria);
            /* e retorna a rua */
            return i+1;
        }
    }

    /* sem ruas possíveis */
    return 0;
}

/* Tenta desfilar uma dada alegoria. */
unsigned desfila_proximo(Pilha *ruas, unsigned qtd_ruas, unsigned alegoria) {
    unsigned i; for (i = 0; i < qtd_ruas; i++) {
        unsigned *proxima_aleg = ver_topo(ruas[i]);

        /* se tiver alguma coisa e for a alegoria buscada */
        if (proxima_aleg != NULL && (uintptr_t) proxima_aleg == alegoria) {
            /* remove da rua */
            desempilhar(ruas[i]);
            /* e retorna a perpendicular onde estava */
            return i+1;
        }
    }

    /* alegoria não encontrada */
    return 0;
}

/*  Como eu usei um cast de inteiro para ponteiro para continuar usando
 * a pilha genérica só que sem ficar alocando memória toda vez, foi
 * necessário escrever essa função vazia para ser chamada no destrutor
 * da pilha.
 */
void desaloca_alegoria(void *alegoria) {
    return;
}

int main(void) {
    unsigned i, ultima_alegoria;
    int capacidade_texto, tamanho_texto;
    bool tem_desfile;
    Pilha *ruas;
    char *texto;

    /* quantidade de alegorias e de ruas */
    unsigned n, r;
    scanf("%u %u", &n, &r);

    /* ruas perpendiculares */
    ruas = malloc(r * sizeof(Pilha));
    for (i = 0; i < r; i++) {
        ruas[i] = constroi_pilha();
    }

    /* Texto de saída.
     *  46 é o tamanho da maior linha escrita em uma operação,
     * considerando o limite de 5 casas decimais pra a alegoria
     * e pra a rua. Cada carro alegórico só pode aparecer em, no
     * máximo, 2 operações, entrada e saída. Então, no pior dos
     * casos, o texto teria que segurar 2 * 46 * n caracteres.
     */
    capacidade_texto = 2 * 46 * n;
    texto = malloc(capacidade_texto * sizeof(char));
    tamanho_texto = 0;

    ultima_alegoria = 0; /* ultima alegoria que já desfilou */

    tem_desfile = true;

    for (i = 0; i < n && tem_desfile; i++) {
        unsigned rua, alegoria;
        scanf("%u", &alegoria);

        /* se for a próxima alegoria a desfilar */
        if (alegoria == ultima_alegoria + 1) {
            ultima_alegoria++;
            escreve_desfile(texto, tamanho_texto, capacidade_texto, alegoria);
        /* se puder ser colocada em uma perpendicular */
        } else if ((rua = encaixa_alegoria(ruas, r, alegoria)) > 0) {
            escreve_entrada(texto, tamanho_texto, capacidade_texto, alegoria, rua);
        } else {
            tem_desfile = false;
        }

        /* tenta resolver as alegorias que estão aguardando */
        while ((rua = desfila_proximo(ruas, r, ultima_alegoria+1)) > 0) {
            escreve_saida(texto, tamanho_texto, capacidade_texto, ++ultima_alegoria, rua);
        }
    }

    if (tem_desfile) {
        printf("Desfile pode ser realizado:\n%s", texto);
    } else {
        printf("Sem desfile.\n");
    }

    /* desaloca a memória usada */
    free(texto);
    for (i = 0; i < r; i++) {
        destroi_pilha(ruas[i], desaloca_alegoria);
    }
    free(ruas);

    return 0;
}
