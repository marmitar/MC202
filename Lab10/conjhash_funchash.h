/**********************
 * CABEÇALHO AUXILIAR *
 **********************
 *  FUNÇÃO DE HASHING *
 **********************/

/* Implementação do xxHash (https://github.com/Cyan4973/xxHash), mas
 * baseado em uma versão simplificada (http://create.stephan-brumme.com/xxhash/),
 * só que usando 16 bits. O algoritmo implementa um método multiplicativo com
 * várias operações binárias por cima.
 */

#include <stdint.h>
#include <stddef.h>

/* chave de 16 bits */
#define HASH_TAM 16
typedef uint16_t chave_t;

/* maior valor possível dessa chave */
#define HASH_MAX UINT16_MAX

#define Byte const uint8_t

/* rotação para a esquerda */
#define rot_esq(num, bits, tam) \
    (((num) << (bits)) | (num) >> (tam - bits))

/* processa uma chave */
#define processa(dado, estado, fator_pre, fator_pos, rot) \
    estado = rot_esq((estado) + (dado) * fator_pre, rot, HASH_TAM) * fator_pos

/* processa 64 bits por vez para acelerar */
#define processa_64bits(dados, estados, fator_pre, fator_pos) \
    *(uint64_t *) estados = rot_esq((*(uint64_t *) estados) + (*(uint64_t *) dados) * fator_pre, 17, 64) * fator_pos
#define BYTES_PROC (64 / 8)
#define CHAVES_PROC (64 / HASH_TAM)

/* a função de hashing propriamente */
static chave_t hash(chave_t semente, const void *dado, size_t tam_bytes) {
    /* tabela de primos usada */
    static const chave_t PRIMO[] = {
        24593, 49157, 12289
    };

    /* ponteiro para analise byte a byte */
    Byte *buffer = dado;

    /* ponto de encerramento do hashing */
    Byte *const ult_byte = buffer + tam_bytes;
    /* ponto final de processamento em 64 bits */
    Byte *const ult_bloco = ult_byte - tam_bytes % BYTES_PROC;

    chave_t resultado = (chave_t) tam_bytes;

    if (tam_bytes >= BYTES_PROC) {
        /* estados intermediários */
        chave_t estado[CHAVES_PROC];
        estado[0] = semente + PRIMO[0] + PRIMO[1];
        estado[1] = semente + PRIMO[1];
        estado[2] = semente;
        estado[3] = semente - PRIMO[0];

        /* processamento em blocos */
        while (buffer < ult_bloco) {
            processa_64bits(buffer, estado, PRIMO[1], PRIMO[0]);
            buffer += BYTES_PROC;
        }

        /* colapsamento no resultado */
        resultado += rot_esq(estado[0], 1, HASH_TAM)
            + rot_esq(estado[1], 5, HASH_TAM)
            + rot_esq(estado[2], 10, HASH_TAM)
            + rot_esq(estado[3], 14, HASH_TAM);

    } else {
        /* caso não tenha bytes o bastante para pelo menos um
        processamento em bloco, usa a semente apenas uma vez */
        resultado += semente + PRIMO[2];
    }

    /* bytes restantes */
    while (buffer < ult_byte) {
        processa(*buffer, resultado, PRIMO[2], PRIMO[1], 9);
        buffer++;
    }

    /* mixagem de bits */
    resultado ^= resultado >> 11;
    resultado *= PRIMO[0];
    resultado ^= resultado >> 5;
    resultado *= PRIMO[2];
    resultado ^= resultado >> 8;

    return resultado;
}

#undef Byte
#undef rot_esq
#undef processa
#undef processa_grupo
#undef BYTES_PROC
#undef CHAVES_PROC