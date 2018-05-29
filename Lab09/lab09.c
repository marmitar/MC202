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

/* comparação entre 'id' dos bancos */
int cmp_trans_id(Dado a, Dado b) {
    /* correção, já que o 'id' pode ser um 'long' */
    long dif = ((Transacao) a)->id_banco -((Transacao) b)->id_banco;
    /* mas a comparação deve retorna 'int' */
    return (dif > 0)? 1 : (dif < 0)? -1 : 0;
}

#define imprime_trans(transac, tarifa) \
    printf("Banco %ld R$ %.2f Tarifa R$ %.2f\n", transac->id_banco, transac->valor, tarifa)

/*   Essa função imprime o acumulado em transferências e o arrecadado em tarifa para
 * o mesmo banco que o da transação inicial. Retorna 
 */
Transacao imprime_acumulado(Treap transacs, Transacao inicial, double tarifa) {
    Transacao trans = pega_minimo(transacs);
    double tarifa_total = tarifa;
    while (trans != NULL && trans->id_banco == inicial->id_banco) {
        inicial->valor += trans->valor;
        tarifa_total += tarifa;

        destroi_trans(trans);

        trans = pega_minimo(transacs);
    }

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

    T = pega_minimo(transacoes);
    while (T != NULL) {
        T = imprime_acumulado(transacoes, T, K);
    }

    destroi_treap(transacoes);
    return EXIT_SUCCESS;
}