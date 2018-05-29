#include <stdio.h>
#include "arv_treap.h"
#include <stdlib.h>

typedef struct _trans {
    long id_banco;
    double valor;
} *Transacao;

/* construtor */
Transacao nova_trans(long banco, double valor) {
    Transacao nova = malloc(sizeof(struct _trans));
    nova->id_banco = banco;
    nova->valor = valor;

    return nova;
}
/* destrutor, que é apenas um 'free' */
Destrutor destroi_trans = (Destrutor) free;

/* comparação entre 'id's dos bancos */
int cmp_trans_id(Dado a, Dado b) {
    /* correção, já que o 'id' pode ser um 'long' */
    long dif = ((Transacao) a)->id_banco -((Transacao) b)->id_banco;
    /* mas a comparação deve retornar 'int' */
    return (dif > 0)? 1 : (dif < 0)? -1 : 0;
}

#define imprime_trans(transac, tarifa) \
    printf("Banco %ld R$ %.2f Tarifa R$ %.2f\n", transac->id_banco, transac->valor, tarifa)

/*   Essa função imprime o acumulado em transferências e o arrecadado em tarifa para
 * o mesmo banco que o da transação inicial, se existir. Retorna a transação que não
 * não foi para esse banco para ser repassada para a função novamente, sem precisar
 * reinserir na treap. Se não existe mais transações, retorna NULL.
 */
Transacao imprime_acumulado(Treap transacs, Transacao inicial, double tarifa) {
    Transacao trans;
    double tarifa_total = 0;

    /* se não recebeu banco inicial, busca o menor */
    if (inicial == NULL) {
        inicial = tira_minimo(transacs);
    }

    tarifa_total += tarifa;

    /* para cada transação do mesmo banco */
    trans = tira_minimo(transacs);
    while (trans != NULL && trans->id_banco == inicial->id_banco) {

        /* acumula os valores no inicial */
        inicial->valor += trans->valor;
        tarifa_total += tarifa;

        /* e remove */
        destroi_trans(trans);

        trans = tira_minimo(transacs);
    }

    /* imprime o acumulado */
    imprime_trans(inicial, tarifa_total);
    destroi_trans(inicial);

    return trans;
}

int main(void) {
    Treap transacoes;
    Transacao T; int i;
    int N; double K;

    transacoes = constroi_treap(cmp_trans_id, destroi_trans);

    scanf("%d %lf", &N, &K);

    for (i = 0; i < N; i++) {
        long I; double R;
        scanf("%ld %lf", &I, &R);

        insere_dado(transacoes, nova_trans(I, R));
    }

    T = NULL; do {
        T = imprime_acumulado(transacoes, T, K);
    } while (T != NULL);

    destroi_treap(transacoes);
    return EXIT_SUCCESS;
}