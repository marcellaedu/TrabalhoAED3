#include <stdio.h>
#include <string.h>
#include "menu.h"

//Exibe as opções disponíveis e lê a opção escolhida pelo usuário.
//Entrada: não possui parâmetros.
//Retorno: número da opção informada; -1 se a leitura falhar.
//Pré-condição: a entrada padrão deve estar disponível.
//Pós-condição: a opção escolhida é retornada ao módulo chamador.
int exibirMenu(void) {
    char linha[32];
    int opcao;
    int c;

    printf("\n===== CADASTRO DE PRODUTOS =====\n");
    printf("1 - Cadastrar produto\n");
    printf("2 - Remover produto\n");
    printf("3 - Atualizar preco\n");
    printf("4 - Atualizar estoque\n");
    printf("5 - Consultar produto\n");
    printf("6 - Listar produtos em ordem\n");
    printf("7 - Imprimir arvore B\n");
    printf("8 - Imprimir nos livres do indice\n");
    printf("9 - Imprimir registros livres dos dados\n");
    printf("10 - Executar operacoes em lote\n");
    printf("0 - Sair\n");
    printf("Opcao: ");

    if (fgets(linha, sizeof(linha), stdin) == NULL)
        return -1;

    if (sscanf(linha, "%d", &opcao) != 1)
        return -1;

    if (strchr(linha, '\n') == NULL) {
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }

    return opcao;
}
