#include <stdio.h>
#include <stdbool.h>

#define STR_MAX 50
typedef char *String;
typedef char StringMax[STR_MAX + 1];

int main(void) {
    StringMax chave;
    bool fim = false;
    while (!fim && scanf("%[^\n]", chave) == 1) {
    }

    if (!fim) {
        return -1;
    }
    return 0;
}