/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 02
 */

#include "vetor.h"

#include <stdlib.h>
#include <stdbool.h>
#include "ordem.h"

/* Gera um vetor vazio. */
Vetor criar_vetor(void) {
    Vetor novo;
    novo.capacidade = novo.tamanho = 0;
    novo.vetor = NULL;
    return novo;
}

/* Esvazia o vetor 'ordens'. */
void destroi_vetor(Vetor *ordens) {
    if (ordens->vetor != NULL) {
        free(ordens->vetor);
        ordens->vetor = NULL;
    }

    ordens->tamanho = 0;
    ordens->capacidade = 0;
}

/* Duplica a capacidade do vetor 'ordens', escopo local. */
static void aumenta_capacidade(Vetor *ordens) {
    int i; /* índice do laço */
    /* como o dobro de zero é zero, a capacidade aumenta para um, nesse caso */
    int nova_capacidade = ordens->capacidade > 0? 2*ordens->capacidade : 1;

    /* gera um novo vetor com o dobro de espaço */
    Ordem *vetor = malloc(nova_capacidade * sizeof(Ordem));
    /* e copia todos as ordens para lá */
    for (i = 0; i < ordens->tamanho; i++) {
        vetor[i] = ordens->vetor[i];
    }
    /* apaga o antigo */
    if (ordens->vetor != NULL) {
        free(ordens->vetor);
    }
    /* e reajusta os atributos */
    ordens->vetor = vetor;
    ordens->capacidade = nova_capacidade;
}

/*  Função de busca binária com 2 fatores de ordenação, acessível apenas nesse arquivo.
 *  Tem 2 modos de operação, definidos pela flag 'maior'. Se estiver desativada, a função encontra a
 * primeira ordem (de menor identidade) com preço buscado, se existir, ou preço seguinte. Já quando a-
 * tivada, a flag faz com que a função sempre encontre a posição onde uma nova orden de preço 'preco'
 * será inserida.
 *  'minimo' e 'maximo' são fatores de recursão e devem ser inicializados na primeira chamada com o me-
 * nor e maior índice a ser buscado, respectivamente.
 */
static int busca_binaria(bool maior, Vetor *ordens, double preco, int minimo, int maximo) {
    int meio = (minimo + maximo) / 2;

    /* fim da recursão */
    if (maximo < minimo) {
        return minimo;
    }

    /* busca na metade com menores valores */
    if (ordens->vetor[meio].preco > preco) {
        return busca_binaria(maior, ordens, preco, minimo, meio-1);
    }
    /* metade maior */
    if (ordens->vetor[meio].preco < preco) {
        return busca_binaria(maior, ordens, preco, meio+1, maximo);
    }

    /* se for o mesmo preço, ainda tem que achar a menor/maior identidade */
    return busca_binaria(maior, ordens, preco, maior? (meio+1) : minimo, maior? maximo : (meio-1));
}

/* Cria e insere uma nova ordem no vetor 'ordens'.
 * Retorna a posição onde foi inserida a ordem.
 */
int insere_ordem(Vetor *ordens, int identidade, int quantidade, double preco) {
    int posicao;
    int i;

    /* aumenta a capacidade, se necessário */
    if (ordens->capacidade == ordens->tamanho) {
        aumenta_capacidade(ordens);
    }

    /* busca a posição a ser inserida */
    posicao = busca_binaria(true, ordens, preco, 0, ordens->tamanho-1);

    /* move o resto para a frente do vetor */
    for (i = ordens->tamanho; i > posicao; i--) {
        ordens->vetor[i] = ordens->vetor[i-1];
    }

    /* gera nova ordem */
    ordens->vetor[posicao].identidade = identidade;
    ordens->vetor[posicao].quantidade = quantidade;
    ordens->vetor[posicao].preco = preco;

    /* atualiza vetor */
    ordens->tamanho++;
    /* retorna a posição onde foi inserida */
    return posicao;
}

/* Reduz a capacidade pela metade, escopo local. */
static void reduz_capacidade(Vetor *ordens) {
    int i;
    /* novo tamanho do vetor, truncado na nova capacidade */
    int novo_tamanho = ordens->tamanho > ordens->capacidade/2? ordens->capacidade/2 : ordens->tamanho;

    /* novo vetor com metade do espaço */
    Ordem *vetor = malloc(ordens->capacidade/2 * sizeof(Ordem));
    /* copia as ordens até o novo tamanho */
    for (i = 0; i < novo_tamanho; i++) {
        vetor[i] = ordens->vetor[i];
    }
    /* libera a memória do vetor antigo */
    if (ordens->vetor != NULL) {
        free(ordens->vetor);
    }

    /* atualiza atributos */
    ordens->vetor = vetor;
    ordens->capacidade /= 2;
    ordens->tamanho = novo_tamanho;
}


/* Remove a ordem em uma dada posição do vetor 'ordens'.
 * Retorna a identidade da ordem removida.
 */
int remove_ordem(Vetor *ordens, int posicao) {
    int i;
    /* armazena a identidade antes de remover */
    int identidade = ordens->vetor[posicao].identidade;

    /* copia todas as ordens seguintes (removendo a atual) */
    for (i = posicao; i < ordens->tamanho-1; i++) {
        ordens->vetor[i] = ordens->vetor[i+1];
    }

    /* reduz o tamanho */
    ordens->tamanho--;
    /* e testa se há muito espaço livre */
    if (ordens->tamanho <= ordens->capacidade / 4) {
        reduz_capacidade(ordens);
    }

    return identidade;
}

/*  Busca uma ordem com o dado preço no vetor 'ordens'.
 *  Retorna a posição da ordem com menor identidade e com esse preço ou -1,
 * se nada for encontrado.
 */
int busca_ordem(Vetor *ordens, double preco) {
    int resultado;
    /* vetor vazio, evita erros */
    if (ordens->vetor == NULL) {
        return -1;
    }
    
    /* usa a busca binária para encontrar um possível resultado */
    resultado = busca_binaria(false, ordens, preco, 0, ordens->tamanho-1);
    /* mas testa se o valor é válido ou não */
    return teste_preco(ordens, resultado, preco)? resultado : -1;
}
