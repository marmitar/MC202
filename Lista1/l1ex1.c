/* Garante que a fila inicie vazia. */
void fila_criar(Fila *fila) {
    fila->tamanho = 0;
    fila->inicio = fila->fim = NULL;
    /* o início é igual ao fim que é igual a NULL */
}

/* Garante que não ocorra vazamento da memória. */
void fila_destruir(Fila *fila) {

    /* percorre a fila */
    No *ptr = fila->inicio;
    while(ptr != NULL) {
        /* liberando a memória de cada nó */
        free(ptr);
        ptr = ptr->proximo;
    }

    /* zera a lista para evitar erros */
    fila_criar(fila);
}

/* Adiciona um novo registro na fila. */
void fila_enfileirar(Fila *fila , Registro  registro) {
    /* novo nó, já inicializado com zeros */
    No *novo = (No *) calloc(1, sizeof(No));
    novo->registro = registro;

    /* insere o nó */
    if (fila->inicio == NULL) {
        fila->inicio = novo;
    } else {
        fila->fim->proximo = novo;
    }

    /* atualiza o ponteiro para o final */
    fila->fim = novo;
    /* e o tamanho */
    fila->tamanho++;
}

/* Remove o elemento mais antigo da fila. */
Registro fila_desenfileirar(Fila *fila) {
    /* registro que deve ser retornado */
    Registro resposta;
    /* ponteiro para o nó que deve ser removido */
    No *ptr = fila->inicio;

    /* pega o registro */
    resposta = ptr->registro;
    /* remove o nó da fila */
    fila->inicio = fila->inicio->proximo;
    fila->tamanho--;
    /* e libera a memória */
    free(ptr);

    return resposta;
}

/* Pega o elemento mais antigo da fila, mas sem removê-lo. */
Registro fila_pegar_proximo(Fila *fila) {
   return fila->inicio->registro;
}

/* Tamanho da fila */
int fila_tamanho(Fila *fila) {
    return fila->tamanho;
}