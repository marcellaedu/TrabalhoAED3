#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "sistema.h"
#include "arvoreb.h"
#include "arquivo.h"
#include "lote.h"

// Exibe o menu e executar a opcao escolhida.
// Entrada: Nao possui parametros.
// Retorno: Nao possui retorno.
// Pre-condicao: Os arquivos devem estar inicializados.
// Pos-condicao: As operacoes escolhidas sao executadas ate o usuario sair.
void exibirMenu()
{
    char linha[32];
    int opcao;
    int c;

    do
    {
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
            return;

        if (strchr(linha, '\n') == NULL)
        {
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            printf("Opcao invalida.\n");
            continue;
        }

        if (sscanf(linha, "%d", &opcao) != 1)
        {
            printf("Opcao invalida.\n");
            continue;
        }

        switch (opcao)
        {
        case 1:
            cadastrarProduto();
            break;
        case 2:
            excluirProduto();
            break;
        case 3:
            atualizarPrecoSistema();
            break;
        case 4:
            atualizarEstoqueSistema();
            break;
        case 5:
            consultarProduto();
            break;
        case 6:
            listarEmOrdem();
            break;
        case 7:
            imprimirArvoreB();
            break;
        case 8:
            imprimirListaLivresIndices();
            break;
        case 9:
            imprimirListaLivresDados();
            break;
        
        case 10:
        {
            char nomeArquivo[100];

            printf("Nome do arquivo de lote: ");

            if (fgets(nomeArquivo, sizeof(nomeArquivo), stdin) == NULL)
            {
                printf("Erro ao ler o nome do arquivo.\n");
                break;
            }

            nomeArquivo[strcspn(nomeArquivo, "\r\n")] = '\0';

            if (nomeArquivo[0] == '\0')
            {
                printf("Nome do arquivo invalido.\n");
                break;
            }

            if (operacoes(nomeArquivo))
                printf("Arquivo de lote processado com sucesso!\n");
            else
                printf("Erro ao processar o arquivo de lote.\n");

            break;
        }

        case 0:
            printf("Encerrando o sistema.\n");
            break;

        default:
            printf("Opcao ainda nao implementada.\n");
            break;
        }

    } while (opcao != 0);
}
