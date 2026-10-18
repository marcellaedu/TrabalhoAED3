#include <stdio.h>
#include <string.h>

#include "sistema.h"
#include "produto.h"
#include "arquivo.h"
#include "arvoreb.h"

// Cadastra um novo produto no sistema.
// Entrada: Dados do produto informados pelo usuario.
// Retorno: 1 se o cadastro for realizado; 0 caso contrario.
// Pre-condicao: Os arquivos devem estar inicializados.
// Pos-condicao: O produto e gravado nos dados e seu codigo e inserido na Arvore B.
int cadastrarProduto(void)
{
    Produto produto = {0};
    char linha[32];
    char extra;
    int posDados;
    int resultado;
    int c;

    printf("\n===== CADASTRAR PRODUTO =====\n");
    printf("Codigo: ");

    if (fgets(linha, sizeof(linha), stdin) == NULL)
        return 0;

    if (strchr(linha, '\n') == NULL)
    {
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
        printf("Codigo invalido.\n");
        return 0;
    }

    if (sscanf(linha, " %d %c", &produto.codigo, &extra) != 1 ||
        produto.codigo <= 0)
    {
        printf("Codigo invalido. Informe um numero inteiro positivo.\n");
        return 0;
    }

    resultado = buscarArvoreB(produto.codigo, &posDados);

    if (resultado == 1)
    {
        printf("Ja existe um produto com esse codigo.\n");
        return 0;
    }

    if (resultado == -1)
    {
        printf("Erro ao consultar a Arvore B.\n");
        return 0;
    }

    if (!lerProduto(&produto))
    {
        printf("Dados do produto invalidos.\n");
        return 0;
    }

    if (!gravarNovoProduto(&produto, &posDados))
    {
        printf("Erro ao gravar o produto.\n");
        return 0;
    }

    resultado = inserirArvoreB(produto.codigo, posDados);

    if (resultado != 1)
    {
        if (!liberarRegistroDados(posDados))
            printf("Aviso: nao foi possivel liberar o registro dos dados.\n");

        if (resultado == 0)
            printf("O codigo ja existe na Arvore B.\n");
        else
            printf("Erro ao inserir o codigo na Arvore B.\n");

        return 0;
    }

    printf("\nProduto cadastrado com sucesso!\n");
    return 1;
}

// Exclui um produto do sistema.
// Entrada: Codigo do produto informado pelo usuario.
// Retorno: 1 se excluido; 0 se nao encontrado ou se ocorrer erro.
// Pre-condicao: Os arquivos devem estar inicializados.
// Pos-condicao: O codigo e removido da Arvore B e o registro de dados liberado quando a operacao e concluida.
int excluirProduto(void)
{
    char linha[32];
    char extra;
    int codigo;
    int posDados;
    int resultado;
    int c;
    RegistroDados registro;

    printf("\n===== EXCLUIR PRODUTO =====\n");
    printf("Codigo do produto: ");

    if (fgets(linha, sizeof(linha), stdin) == NULL)
        return 0;

    if (strchr(linha, '\n') == NULL)
    {
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
        printf("Entrada invalida.\n");
        return 0;
    }

    if (sscanf(linha, " %d %c", &codigo, &extra) != 1 ||
        codigo <= 0)
    {
        printf("Codigo invalido.\n");
        return 0;
    }

    resultado = buscarArvoreB(codigo, &posDados);

    if (resultado == 0)
    {
        printf("Produto nao encontrado.\n");
        return 0;
    }

    if (resultado == -1)
    {
        printf("Erro ao consultar a Arvore B.\n");
        return 0;
    }

    if (!lerRegistroDados(posDados, &registro))
    {
        printf("Erro ao ler os dados do produto.\n");
        return 0;
    }

    resultado = removerArvoreB(codigo);

    if (resultado != 1)
    {
        printf("Erro ao remover o produto da Arvore B.\n");
        return 0;
    }

    if (!liberarRegistroDados(posDados))
    {
        printf("O produto foi removido da Arvore B, mas nao foi "
               "possivel liberar seu registro de dados.\n");
        return 0;
    }

    printf("Produto \"%s\" excluido com sucesso!\n",
           registro.produto.nome);

    return 1;
}

/**
 Consulta um produto pelo codigo.
 Entrada: Codigo do produto informado pelo usuario.
 Retorno: 1 se a consulta for realizada; 0 caso contrario.
 Pre-condicao: Os arquivos devem estar inicializados.
 Pos-condicao: Os dados do produto encontrado sao exibidos.
 */
int consultarProduto(void)
{
    char linha[32];
    char extra;
    int codigo;
    int posDados;
    int resultado;
    RegistroDados registro;
    int c;

    printf("\n===== CONSULTAR PRODUTO =====\n");
    printf("Digite o codigo:");
    if (fgets(linha, sizeof(linha), stdin) == NULL)
        return 0;

    if (sscanf(linha, " %d %c", &codigo, &extra) != 1 ||
        codigo <= 0)
    {
        printf("Codigo invalido.\n");
        return 0;
    }

    resultado = buscarArvoreB(codigo, &posDados);

    if (resultado == 0)
    {
        printf("Produto nao encontrado.\n");
        return 0;
    }

    if (resultado == -1)
    {
        printf("Erro ao consultar a Arvore B.\n");
        return 0;
    }

    if (!lerRegistroDados(posDados, &registro))
    {
        printf("Erro ao ler os dados do produto.\n");
        return 0;
    }

    imprimirProduto(registro.produto);
    return 1;
}

// Atualiza o preco de um produto.
// Entrada: Codigo do produto e novo preco informados pelo usuario.
// Retorno: 1 se a atualizacao for realizada; 0 caso contrario.
// Pre-condicao: Os arquivos devem estar inicializados.
// Pos-condicao: O novo preco e gravado no arquivo de dados.
int atualizarPrecoSistema(void)
{
    char linha[32];
    char extra;
    int codigo, posDados, resultado, c;
    float novoPreco;
    RegistroDados registro;

    printf("\n===== ATUALIZAR PRECO =====\n");
    printf("Codigo do produto: ");

    if (fgets(linha, sizeof(linha), stdin) == NULL)
        return 0;

    if (sscanf(linha, " %d %c", &codigo, &extra) != 1 ||
        codigo <= 0)
    {
        printf("Codigo invalido.\n");
        return 0;
    }

    resultado = buscarArvoreB(codigo, &posDados);

    if (resultado != 1)
    {
        printf(resultado == 0 ? "Produto nao encontrado.\n"
                              : "Erro na consulta.\n");
        return 0;
    }

    if (!lerRegistroDados(posDados, &registro))
    {
        printf("Erro ao ler produto.\n");
        return 0;
    }

    printf("Preco atual: R$ %.2f\n", registro.produto.preco);
    printf("Novo preco: ");

    if (fgets(linha, sizeof(linha), stdin) == NULL)
        return 0;

    if (strchr(linha, '\n') == NULL)
    {
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
        printf("Entrada invalida.\n");
        return 0;
    }
    linha[strcspn(linha, "\r\n")] = '\0';

    if (!converterPreco(linha, &novoPreco) ||
        !atualizarPrecoProduto(&registro.produto, novoPreco))
    {
        printf("Preco invalido.\n");
        return 0;
    }

    if (!gravarRegistroDados(posDados, &registro))
    {
        printf("Erro ao atualizar preco.\n");
        return 0;
    }

    printf("Preco atualizado com sucesso!\n");
    return 1;
}

// Atualiza o estoque de um produto.
// Entrada: Codigo do produto e novo estoque.
// Retorno: 1 se atualizado; 0 caso contrario.
// Pre-condicao: Os arquivos devem estar inicializados.
// Pos-condicao: O novo estoque e gravado no arquivo.
int atualizarEstoqueSistema(void)
{
    char linha[32];
    int codigo, posDados, estoque, c;
    RegistroDados registro;

    printf("\n===== ATUALIZAR ESTOQUE =====\n");
    printf("Codigo: ");

    if (!fgets(linha, sizeof(linha), stdin))
        return 0;

    if (strchr(linha, '\n') == NULL)
    {
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
        printf("Entrada invalida.\n");
        return 0;
    }

    if (sscanf(linha, "%d", &codigo) != 1 || codigo <= 0)
    {
        printf("Codigo invalido.\n");
        return 0;
    }

    if (buscarArvoreB(codigo, &posDados) != 1 ||
        !lerRegistroDados(posDados, &registro))
    {
        printf("Produto nao encontrado ou erro na consulta.\n");
        return 0;
    }

    printf("Estoque atual: %d\n", registro.produto.estoque);
    printf("Novo estoque: ");

    if (!fgets(linha, sizeof(linha), stdin))
        return 0;

    if (strchr(linha, '\n') == NULL)
    {
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
        printf("Entrada invalida.\n");
        return 0;
    }

    if (sscanf(linha, "%d", &estoque) != 1 ||
        !atualizarEstoqueProduto(&registro.produto, estoque))
    {
        printf("Estoque invalido.\n");
        return 0;
    }

    if (!gravarRegistroDados(posDados, &registro))
    {
        printf("Erro ao gravar.\n");
        return 0;
    }

    printf("Estoque atualizado!\n");
    return 1;
}
