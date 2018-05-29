#include <stdio.h>
#include "arv_treap.h"
#include <stdlib.h>

typedef struct _trans {
    long long id_banco;
    double valor;
} *Transacao;

Transacao nova_trans(long long banco, double valor) {
    Transacao nova = malloc(sizeof(struct _trans));
    nova->id_banco = banco;
    nova->valor = valor;

    return nova;
}
int cmp_trans(Dado a, Dado b) {
    return ((Transacao) a)->id_banco -((Transacao) b)->id_banco;
}
Destrutor destroi_trans = (Destrutor) free;

int main(void) {
    Treap transacoes = constroi_treap(cmp_trans, destroi_trans);

    int N; double K;
    scanf("%d %lf", &N, &K);

    for (int i = 0; i < N; i++) {
        long long I; double R;
        scanf("%lld %lf", &I, &R);

        insere_dado(transacoes, nova_trans(I, R));
    }

    destroi_treap(transacoes);

    return 0;
}