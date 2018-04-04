/* Inicialização dos valores, zerando-os. */
void  vetor_inicializar(int **vec , int  tamanho) {
    size_t i;
    /* percorre o vetor de ponteiros */
    for (i = 0; i < tamanho; i++) {
        /* e inicializa o valor para onde está sendo apontado */
        *(vec[i]) = 0;
    }
}

/* Libera a memória. */
void  vetor_desalocar(int **vec , int  tamanho) {
    size_t i;
    /* percorre o vetor */
    for (i = 0; i < tamanho; i++) {
        /*  liberando a memória de cada ponteiro */
        free(vec[i]);
    }
    /* e então libera a do vetor */
    free(vec);
}