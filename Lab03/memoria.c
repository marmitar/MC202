/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 03
 */

/* Biblioteca de um simulador de memória para o laboratório. */
#include "memoria.h"

#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Todas as funções de macro são usadas somente para coisas simples e têm escopo limitado */

/* Testa se um nó é vazio ou não */
#define eh_vazio(ptr) \
    (ptr == NULL)

/* Encontra o mínimo ou o máximo entre os dois valores */
#define min(a, b) \
    (((a) < (b))? (a) : (b))
#define max(a, b) \
    (((a) > (b))? (a) : (b))

/* Funções de segmento de memória. Todas com escopo local. */
/* Construtor e destrutor de segmento de memória. Escopo direto local. */
static void *constroi_seg(Mem_pos endereco, Mem_pos tamanho) {
    Segmento novo = malloc(sizeof(struct _seg));
    novo->endereco = endereco;
    novo->tamanho = tamanho;
    return (void *) novo;
}
static void destroi_seg(const void *segmento) {
    free((Segmento) segmento);
}
/* Comparador de segmentos de memória. Escopo direto local. */
static int compara_seg(const void *primeiro, const void *segundo) {
    /* compara apenas os endereços dos segmentos */
    return ((Segmento) primeiro)->endereco - ((Segmento) segundo)->endereco;
}
/* Inserção e remoção de segmento da memória. Escopo limitado. 
   Apenas invólucros das funções de listas genéricas. */
#define insere_seg(mem, end, tam) \
    if (((tam) > 0) && (((end) + (tam)) <= (mem).tamanho)) { /* impede a inserção de segmentos vazios ou fora da capacidade */ \
        insere_dado((mem).segmentos, constroi_seg(end, tam), compara_seg); \
    } (void) 0 /* encaixe para o final de declaração, ';', em C */
#define remove_seg(mem, dado) \
    remove_dado((mem).segmentos, dado, compara_seg, destroi_seg)

/* Construtor. */
Memoria constroi_memoria(Mem_pos tamanho) {
    struct _mem nova;
    nova.segmentos = constroi_lista();
    nova.tamanho = tamanho;

    insere_seg(nova, 0, tamanho); /* inicializa com memória inteira livre em um segmento */
    return nova;
}
/* Destrutor. */
void destroi_memoria(Memoria mem) {
    destroi_lista(mem.segmentos, destroi_seg);
}

/* Aloca segmento da memória.
 * Retorna se foi possível alocar memória ou não.
 */
bool aloca_memoria(Memoria mem, Mem_pos tamanho) {
    No ptr = mem.segmentos->cab; /* ponteiro que percorre a lista ligada */

    /* encontra o último nó ou o primeiro nó com memória o bastante para a alocação */
    while (! eh_vazio(ptr) && tamanho > ((Segmento) ptr->dado)->tamanho) {
        ptr = ptr->prox;
    }

    /* sem espaço na memória */
    if (eh_vazio(ptr)) {
        return false;
    }

    /* ajuste da lista de segmentos */
    insere_seg(mem, ((Segmento) ptr->dado)->endereco + tamanho, ((Segmento) ptr->dado)->tamanho - tamanho);
    remove_seg(mem, ptr->dado);
    return true;
}

/* Desalocação de memória.
 * Retorna se ocorreu desalocação repetida de uma mesma célula de memória.
 */
bool desaloca_memoria(Memoria mem, Mem_pos endereco, Mem_pos tamanho) {
    No anterior = NULL; /* ponteiro para um nó antes */
    No proximo = mem.segmentos->cab; /* ponteiro que percorre a lista */

    bool desalocacao_multipla = false; /* teste te desalocação repetida */

    /* encontra o último nó ou o primeiro segmento com endereço maior ao procurado */
    while (! eh_vazio(proximo) && (((Segmento) proximo->dado)->endereco <= endereco)) {
        anterior = proximo;
        proximo = proximo->prox;
    }

    /* se existe segmento livre antes do que será desalocado */
    if (! eh_vazio(anterior)) {
        /* calcula a possível sobreposição entre os segmentos */
        int sobreposicao = min((((Segmento) anterior->dado)->endereco + ((Segmento) anterior->dado)->tamanho), endereco + tamanho)
            - max(((Segmento) anterior->dado)->endereco, endereco);

        /* se existe sobreposiçao ou, pelo menos, vizinhança com o segmento que será desalocado */
        if (sobreposicao >= 0) {
            /* teste de desalocação repetida */
            desalocacao_multipla |= sobreposicao > 0;

            /* aglutina os segmentos em um */
            endereco = min(((Segmento) anterior->dado)->endereco, endereco);
            tamanho += ((Segmento) anterior->dado)->tamanho - sobreposicao;
            /* e remove o anterior */
            remove_seg(mem, anterior->dado);
        }
    }
    /* mesma coisa, mas para o segmento seguinte */
    if (! eh_vazio(proximo)) {
        /* sobreposição entre os segmentos */
        int sobreposicao = min(endereco + tamanho, ((Segmento) proximo->dado)->endereco + ((Segmento) proximo->dado)->tamanho)
            - max(((Segmento) proximo->dado)->endereco, endereco);

        /* se sobrepõe ou faz fronteira com o segmento a desalocar */
        if (sobreposicao >= 0) {
            /* testa a sobreposição */
            desalocacao_multipla |= sobreposicao > 0;

            /* calcula o tamanho dos segemntos aglutinados */
            tamanho += ((Segmento) proximo->dado)->tamanho - sobreposicao;
            /* e remove o seguinte */
            remove_seg(mem, proximo->dado);
        }
    }

    /* por fim, insere o novo segmento livre, com os segmentos adjacentes algutinados, caso existam */
    insere_seg(mem, endereco, tamanho);

    return desalocacao_multipla; /* para a correção de erros */
}

/* Realocação de memória.
 * Retorna se foi possível alocar a nova memória.
 */
bool realoca_memoria(Memoria mem, Mem_pos endereco, Mem_pos tamanho, Mem_pos novo_tamanho) {
    No ptr = mem.segmentos->cab; /* ponteiro para percorrer a lista */

    /* redução da memória alocada */
    if (novo_tamanho <= tamanho) {
        /* apenas desaloca o espaço que não será mais utilizado */
        desaloca_memoria(mem, endereco + novo_tamanho, tamanho - novo_tamanho);
        return true;
    }

    /* encontra o último segmento ou primeiro com edereço maior */
    while (! eh_vazio(ptr) && (((Segmento) ptr->dado)->endereco <= endereco)) {
        ptr = ptr->prox;
    }

    /* se existe um seguimento livre após o endereço, se é adjacente ao segmento a ser expandido e se tem espaço o bastante */
    if (! eh_vazio(ptr) && ((Segmento) ptr->dado)->endereco == (endereco + tamanho) && ((Segmento) ptr->dado)->tamanho >= (novo_tamanho - tamanho)) {
        /* insere um segmento livre com menos espaço */
        insere_seg(mem, ((Segmento) ptr->dado)->endereco + novo_tamanho - tamanho, ((Segmento) ptr->dado)->tamanho - novo_tamanho + tamanho);
        /* e tira o segmento que foi utilizado */
        remove_seg(mem, ptr->dado);
        return true;
    }

    /* se nenhum dos caso acima */
    /* então aloca a nova memória em outra posição */
    bool resultado = aloca_memoria(mem, novo_tamanho);
    /* e desaloca a antiga */
    desaloca_memoria(mem, endereco, tamanho);

    /* retorna o resultado dessa alocação */
    return resultado;
}

/* Impressão dos segmentos de memória livres */
void imprime_segmentos(Memoria mem) {
    No ptr;
    printf("Segmentos livres da heap:\n");

    /* percorre e imprime todos os nós da lista */
    for (ptr = mem.segmentos->cab; ! eh_vazio(ptr); ptr = ptr->prox) {
        printf("(%lu, %lu)\n", ((Segmento) ptr->dado)->endereco, ((Segmento) ptr->dado)->tamanho);
    }
}

/* evita o "vazamento" das funções macro */
#undef eh_vazio
#undef min
#undef max
#undef insere_seg
#undef remove_seg
