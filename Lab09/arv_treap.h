#ifndef _ARV_TREAP_H
#define _ARV_TREAP_H

typedef void *Dado;
typedef int (*Comparador)(Dado, Dado);
typedef void (*Destrutor)(Dado);

typedef struct _treap *Treap;

Treap constroi_treap(Comparador, Destrutor);
void destroi_treap(Treap);

void insere_dado(Treap, Dado);
/* Dado remove_dado(Treap, Dado); */

Dado pega_minimo(Treap);

#endif /* _ARV_TREAP_H */