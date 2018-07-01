/* mesmo arquivo que o auxiliar */
#include <stdlib.h>
#include <stdio.h>
#include "fila_prio.h"

void subir_no_heap(FilaPrio *fp, int i);
void descer_no_heap(FilaPrio *fp, int i);
void trocar(FilaPrio *fp, int i, int j);

FilaPrio *criar_fila_prio(int tamanho)
{
  FilaPrio *fp = malloc(sizeof(FilaPrio));
  fp->indice = malloc(tamanho*sizeof(int));
  fp->heap = malloc(tamanho*sizeof(int));
  fp->prioridade = malloc(tamanho*sizeof(int));
  fp->tamanho_atual = 0;
  fp->tamanho_maximo = tamanho;
  return fp;
}

void destruir_fila_prio(FilaPrio *fp)
{
  if (fp != NULL) {
    free(fp->indice);
    free(fp->heap);
    free(fp->prioridade);
    free(fp);
  }
}

int extrair_minimo(FilaPrio *fp)
{
  const int fim = --(fp->tamanho_atual);
  trocar(fp, 0, fim);
  descer_no_heap(fp, 0);
  return fp->heap[fim];
}

int inserir(FilaPrio *fp, int v, int p)
{
  int cheio = fp->tamanho_atual == fp->tamanho_maximo;

  if (!cheio) { /* não está cheio!! */
    const int fim = fp->tamanho_atual++;
    fp->indice[v] = fim;
    fp->heap[fim] = v;
    fp->prioridade[v] = p;
    subir_no_heap(fp, fim);
  }

  return cheio;
}

void diminuir_prioridade(FilaPrio *fp, int v, int p)
{
  fp->prioridade[v] = p;
  subir_no_heap(fp, fp->indice[v]);
}

int eh_vazia(FilaPrio *fp)
{
  return !fp->tamanho_atual;
}

int prioridade(FilaPrio *fp, int v)
{
  return fp->prioridade[v];
}

void subir_no_heap(FilaPrio *fp, int i)
{
  if (i > 0) { /* não é o nó raiz!! */

    const int pai = (i-1)/2;

    if (prioridade(fp, fp->heap[pai]) > prioridade(fp, fp->heap[i])) {
      trocar(fp, pai, i);
      subir_no_heap(fp, pai);
    }
  }
}

void descer_no_heap(FilaPrio *fp, int i)
{
  const int filho_esquerdo = 2*i + 1;
  const int filho_direito  = 2*i + 2;

  if (filho_esquerdo < fp->tamanho_atual) {

    int menor_filho = filho_esquerdo;

    if (filho_direito < fp->tamanho_atual) {

      int prioridade_esquerdo = prioridade(fp, fp->heap[filho_esquerdo]);
      int prioridade_direito  = prioridade(fp, fp->heap[filho_direito]);

      if (prioridade_esquerdo > prioridade_direito) {
        menor_filho = filho_direito;
      }
    }

    if (prioridade(fp, fp->heap[i]) > prioridade(fp, fp->heap[menor_filho])) {
      trocar(fp, i, menor_filho);
      descer_no_heap(fp, menor_filho);
    }
  }
}

void trocar(FilaPrio *fp, int i, int j)
{
  int temporario = fp->heap[i];
  fp->heap[i] = fp->heap[j];
  fp->heap[j] = temporario;
  fp->indice[fp->heap[i]] = i;
  fp->indice[fp->heap[j]] = j;
}