p_no transforma_em_lista(p_no raiz) {
    p_no esq_ini, esq_fim;
    p_no dir_ini, dir_fim;

    /* uma árvore vazia já é uma lista ordenada */
    if (raiz == NULL) {
        return NULL;
    }

    /* reordena a sub-árvore esquerda em uma lista */
    esq_ini = transforma_em_lista(raiz->esq);
    /* se a lista for vazia */
    if (esq_ini == NULL) {
        /* troca a lista pela raiz */
        esq_ini = esq_fim = raiz;
    } else {
        /* senão, encontra o final da lista */
        esq_fim = esq_ini->esq;
    }

    /* faz o mesmo com a direita */
    dir_ini = transforma_em_lista(raiz->dir);
    if (dir_ini == NULL) {
        dir_ini = dir_fim = raiz;
    } else {
        dir_fim = dir_ini->esq;
    }

    /* conecta a lista esquerda na raiz */
    esq_fim->dir = raiz;
    raiz->esq = esq_fim;
    /* e a raiz na lista direita */
    dir_ini->esq = raiz;
    raiz->dir = dir_ini;

    /* resolve a circularidade da lista */
    dir_fim->dir = esq_ini;
    esq_ini->esq = dir_fim;

    return esq_ini;
}