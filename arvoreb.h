#ifndef ARVOREB_H
#define ARVOREB_H

#define ORDEM 5
#define MAX_CHAVES (ORDEM - 1)

typedef struct NoB {
    int numChaves;
    int chaves[MAX_CHAVES];
    int posDados[MAX_CHAVES];
    int filhos[ORDEM];
    int proxLivre;
} NoB;

#endif
