/* Se for completa, retorna a profundidade da árvore + 1,
 * se for cheia, retorna o negativo disso,
 * senão, retorna 0.
 */
int eh_completa(p_no raiz) {
    int prof_esq, prof_dir;
    bool esq_cheia, dir_cheia;
    bool cheia, completa;

    /* árvore vazia é cheia com profundidade 0 */
    if (raiz == NULL) {
        return -1;
    }

    /* análise da profundidade, da completude e da plenitude
      das sub-árvores esquerda e direita */

    prof_esq = eh_completa(raiz->esq);
    if (prof_esq < 0) {
        esq_cheia = true;
        prof_esq = -prof_esq;
    } else if (prof_esq > 0) {
        esq_cheia = false;
    } else {
        return 0;
    }

    prof_dir = eh_completa(raiz->dir);
    if (prof_dir < 0) {
        dir_cheia = true;
        prof_dir = -prof_dir;
    } else if (prof_dir > 0) {
        dir_cheia = false;
    } else {
        return 0;
    }

    /* com isso, sabemos que as sub-árvores são completas e
      podemos analisar a completude da árvore principal */

    cheia = esq_cheia && dir_cheia && prof_esq == prof_dir;


    completa = (esq_cheia && prof_esq == prof_dir)
        || (dir_cheia && prof_esq == prof_dir+1);

    /* com base na plenitude e na completude,
      retornamos o valor */

    if (completa) {
        return (cheia? -1 : 1) * (prof_esq + 1);
    } else {
        return 0;
    }
}