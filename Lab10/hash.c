#include <stdint.h>
#include <string.h>
#include <limits.h>

#define HASH_TAM 16

typedef uint16_t chave_t;

#define _rot_esq(num, bits) \
    (((num) << (bits)) | (num) >> (HASH_TAM - bits))

#define processa(dado, estado, fator_pre, fator_pos) \
    estado = _rot_esq(estado + dado * fator_pre, _ROT_BITS) * fator_pos

#define processa_todos(dados, estados, fator_pre, fator_pos) \
    processa(dados[0], estados[0], fator_pre, fator_pos); \
    processa(dados[1], estados[1], fator_pre, fator_pos); \
    processa(dados[2], estados[2], fator_pre, fator_pos); \
    processa(dados[3], estados[3], fator_pre, fator_pos)
#define _ROT_BITS 7
#define DADOS_PROC 4

#define BYTES_P_PROC (DADOS_PROC * HASH_TAM / CHAR_BIT)

static chave_t _hash(chave_t semente, const char *str) {
    static const chave_t PRIMO[DADOS_PROC+1] = {
        3, 5, 7, 11, 13
    };

    chave_t estado[DADOS_PROC] = {
        semente + PRIMO[0] + PRIMO[1],
        semente + PRIMO[1],
        semente,
        semente - PRIMO[0]
    };

    size_t tam = strlen(str);

    chave_t resultado = (chave_t) tam;
    if (tam > DADOS_PROC) {
        while (tam > DADOS_PROC) {
            processa_todos(str, estado, PRIMO[1], PRIMO[0]);
            tam -= DADOS_PROC;
        }
        tam += _rot_esq(estado[0], 1) +
            _rot_esq(estado[1], 5) +
            _rot_esq(estado[2], 10) +
            _rot_esq(estado[3], 14);
    } else {
        tam += estado[2] + PRIMO[DADOS_PROC];
    }

    return resultado;
}