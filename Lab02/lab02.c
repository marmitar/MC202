/* Tiago de Paula Alves
 * RA 187679
 * 2018s1 MC202 A
 * Lab 02
 */

#include <stdio.h>
#include "vetor.h"

int main(void) {
    int total_ordens = 0; /* total de ordens lidas */
    char operacao; /* tipo de operação, '#' para encerrar, 'V' para
    venda e 'C' (ou qualquer outra coisa, na verdade) para compra */

    Vetor compras, vendas;
    /* inicializa o vetor vazio */
    compras = criar_vetor();
    vendas = criar_vetor();

    while (scanf(" %c", &operacao) > 0 && operacao != '#') {
        int ordem_casada; /* posição da ordem casada no vetor complementar */
        int quantidade; /* quantidade de criptomoedas da ordem */
        double preco; /* preço das criptomoedas */
        scanf("%d %lf", &quantidade, &preco); 

        total_ordens++; /* nova ordem lida */

        /* procura uma ordem casada */
        ordem_casada = busca_ordem(operacao == 'V'? &compras : &vendas, preco);
        /* até esgotar a ordem ou não ter mais ordem casada */
        while (quantidade > 0 && ordem_casada >= 0) {
            /* tira as criptomoedas da ordem casada e armazena o resultado em uma nova variável */
            int resultado = (operacao == 'V'? compras : vendas).vetor[ordem_casada].quantidade -= quantidade;

            /* imprime a transação entre as ordens */
            printf("ordem %d vende para ordem %d a quantidade %d por %.02f\n",              \
                operacao == 'V'? total_ordens : vendas.vetor[ordem_casada].identidade,      \
                operacao == 'V'? compras.vetor[ordem_casada].identidade : total_ordens,     \
                quantidade + (resultado < 0? resultado : 0),                                \
                preco);

            /* não sobrou nada na ordem casada */
            if (resultado <= 0) {
                /* quantidade restante */
                quantidade = abs(resultado);
                /* encerra ordem casada */
                printf("ordem %d concluida\n", remove_ordem(operacao == 'V'? &compras : &vendas, ordem_casada));
                /* procura nova ordem casada */
                if (! teste_preco(operacao == 'V'? &compras : &vendas, ordem_casada, preco)) {
                    ordem_casada = -1;
                }
            }
            /* não sobrou nada na nova ordem */
            if (resultado >= 0) {
                quantidade = 0;
                printf("ordem %d concluida\n", total_ordens);
            }
        }

        /* se ainda tem algo na nova ordem, armazena no vetor certo */
        if (quantidade > 0) {
            insere_ordem(operacao == 'V'? &vendas : &compras, total_ordens, quantidade, preco);
        }
    }

    /* limpa a memória usada e encerra */
    destroi_vetor(&compras);
    destroi_vetor(&vendas);
    return 0;
}