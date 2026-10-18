#include <stdio.h>
#include "menu.h"
#include "arquivo.h"

int main()
{
    if (!inicializarArquivos()) {
    printf("Erro ao inicializar os arquivos.\n");
    return 1;
}

exibirMenu();
}
