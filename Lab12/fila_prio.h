#ifndef FILA_PRIO_H
#define FILA_PRIO_H

typedef struct FilaPrio FilaPrio;

struct FilaPrio
{
  int *indice;
  int *heap;
  int *prioridade;
  int tamanho_atual;
  int tamanho_maximo;
};

FilaPrio *criar_fila_prio(int tamanho);
void destruir_fila_prio(FilaPrio *fp);

int inserir(FilaPrio *fp, int v, int p);
int extrair_minimo(FilaPrio *fp);

void diminuir_prioridade(FilaPrio *fp, int v, int p);

int eh_vazia(FilaPrio *fp);
int prioridade(FilaPrio *fp, int v);

#endif