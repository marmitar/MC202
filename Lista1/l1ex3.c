/* Posição no mapa (em forma matricial) */
typedef struct _posicao {
    unsigned linha, coluna;
    struct _posicao *proxima;
} *Posicao;

/* Pilha de posições */
typedef struct _pilha {
    Posicao topo; /* topo da pilha */
    size_t tamanho;
} *Pilha;