
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lote.h"
#include "produto.h"
#include "arquivo.h"
#include "arvoreb.h"

//Objetivo: Separar os campos de uma linha, preservando campos vazios.
//Entrada: Linha do lote, vetor de campos e limite de campos.
//Retorno: Quantidade de campos ou -1 se ultrapassar o limite.
//Pre-condicao: A linha deve ser modificavel.
//Pos-condicao: Os campos ficam separados por ponteiros para a propria linha.
static int separarCampos(char *linha, char *campos[], int limite)
{
    int quantidade = 1;
    char *p = linha;

    campos[0] = linha;

    while (*p != '\0')
    {
        if (*p == ';')
        {
            *p = '\0';

            if (quantidade >= limite)
                return -1;

            campos[quantidade++] = p + 1;
        }

        p++;
    }

    return quantidade;
}

//Objetivo: Inserir um produto a partir dos dados do lote.
//Entrada: Vetor com os campos da operacao de insercao.
//Retorno: 1 se a insercao for realizada; 0 caso contrario.
//Pre-condicao: Os campos devem seguir o formato de insercao.
//Pos-condicao: O produto e inserido no arquivo e na arvore B.
static int inserirProdutoLote(char *campos[])
{
    Produto produto;
    int posicao;
    char *fim;
    long valor;

    produto.codigo = (int)strtol(campos[1], &fim, 10);

    if (campos[1][0] == '\0' || *fim != '\0' ||
        produto.codigo <= 0)
        return 0;

    if (strlen(campos[2]) >= sizeof(produto.nome) ||
        strlen(campos[3]) >= sizeof(produto.marca) ||
        strlen(campos[4]) >= sizeof(produto.categoria))
        return 0;

    strcpy(produto.nome, campos[2]);
    strcpy(produto.marca, campos[3]);
    strcpy(produto.categoria, campos[4]);

    valor = strtol(campos[5], &fim, 10);

    if (campos[5][0] == '\0' || *fim != '\0' || valor < 0)
        return 0;

    produto.estoque = (int)valor;

    if (!converterPreco(campos[6], &produto.preco))
        return 0;

    if (buscarArvoreB(produto.codigo, &posicao) != 0)
        return 0;

    if (!gravarNovoProduto(&produto, &posicao))
        return 0;

    if (!inserirArvoreB(produto.codigo, posicao))
    {
        liberarRegistroDados(posicao);
        return 0;
    }

    return 1;
}

//Objetivo: Alterar o estoque e/ou o preco de um produto pelo lote.
//Entrada: Vetor com os campos da operacao de alteracao.
//Retorno: 1 se a alteracao for realizada; 0 caso contrario.
//Pre-condicao: Os campos devem seguir o formato de alteracao.
//Pos-condicao: Somente os campos informados sao alterados.
static int alterarProdutoLote(char *campos[])
{
    int codigo;
    int posicao;
    char *fim;
    long valor;
    RegistroDados registro;

    codigo = (int)strtol(campos[1], &fim, 10);

    if (campos[1][0] == '\0' || *fim != '\0' || codigo <= 0)
        return 0;

    if (campos[2][0] == '\0' && campos[3][0] == '\0')
        return 0;

    if (buscarArvoreB(codigo, &posicao) != 1)
        return 0;

    if (!lerRegistroDados(posicao, &registro))
        return 0;

    if (campos[2][0] != '\0')
    {
        valor = strtol(campos[2], &fim, 10);

        if (*fim != '\0' || valor < 0)
            return 0;

        registro.produto.estoque = (int)valor;
    }

    if (campos[3][0] != '\0')
    {
        if (!converterPreco(campos[3], &registro.produto.preco))
            return 0;
    }

    return gravarRegistroDados(posicao, &registro);
}

//Objetivo: Remover um produto a partir dos dados do lote.
//Entrada: Vetor com os campos da operacao de remocao.
//Retorno: 1 se a remocao for realizada; 0 caso contrario.
//Pre-condicao: O codigo deve ser valido e corresponder a um produto existente.
//Pos-condicao: O produto e removido da arvore B e seu registro e liberado.
static int removerProdutoLote(char *campos[])
{
    int codigo;
    int posicao;
    char *fim;

    codigo = (int)strtol(campos[1], &fim, 10);

    if (campos[1][0] == '\0' || *fim != '\0' || codigo <= 0)
        return 0;

    if (buscarArvoreB(codigo, &posicao) != 1)
        return 0;

    if (!removerArvoreB(codigo))
        return 0;

    return liberarRegistroDados(posicao);
}

//Objetivo: Identificar e executar a operacao indicada na linha.
//Entrada: Linha do arquivo de lote.
//Retorno: 1 se a operacao for executada; 0 caso contrario.
//Pre-condicao: A linha deve seguir o formato de I, A ou R.
//Pos-condicao: A operacao correspondente e executada.
static int executarOperacaoLote(char *linha)
{
    char *campos[7];
    int quantidade;

    linha[strcspn(linha, "\r\n")] = '\0';

    if (linha[0] == '\0')
        return 1;

    quantidade = separarCampos(linha, campos, 7);

    if (quantidade < 0)
        return 0;

    //Insercao: I;codigo;nome;marca;categoria;estoque;preco
    if (strcmp(campos[0], "I") == 0)
    {
        if (quantidade != 7)
            return 0;

        return inserirProdutoLote(campos);
    }

    //Alteracao: A;codigo;estoque;preco
    if (strcmp(campos[0], "A") == 0)
    {
        if (quantidade != 4)
            return 0;

        return alterarProdutoLote(campos);
    }

    //Remocao: R;codigo
    if (strcmp(campos[0], "R") == 0)
    {
        if (quantidade != 2)
            return 0;

        return removerProdutoLote(campos);
    }

    return 0;
}

//Objetivo: Abrir e processar todas as linhas do arquivo de lote.
//Entrada: Nome do arquivo de lote.
//Retorno: 1 se todas as operacoes forem processadas; 0 se houver erro.
//Pre-condicao: O arquivo deve existir e seguir o formato esperado.
//Pos-condicao: As operacoes sao executadas ate o fim ou ate ocorrer erro.
int operacoes(const char *nomeArquivo){
    FILE *arquivo;
    char linha[512];

    arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL)
        return 0;
int numeroLinha = 0;

while (fgets(linha, sizeof(linha), arquivo) != NULL)
{
    numeroLinha++;

    if (!executarOperacaoLote(linha))
    {
        printf("Erro na linha %d do arquivo de lote.\n", numeroLinha);
        fclose(arquivo);
        return 0;
    }
}

    if (ferror(arquivo))
    {
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);
    return 1;
}
