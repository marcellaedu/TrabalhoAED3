#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "produto.h"

//Remove a quebra de linha do final de uma string, quando existir.
//Entrada: string que pode conter uma quebra de linha.
//Retorno: não possui retorno.
//Pré-condição: texto deve apontar para uma string modificável.
//Pós-condição: a quebra de linha final é removida, se existir.
static void removerQuebraLinha(char *texto) {
    texto[strcspn(texto, "\n")] = '\0';
}

//Lê uma linha de texto e descarta caracteres excedentes.
//Entrada: buffer de destino e tamanho máximo do buffer.
//Retorno: 1 se a leitura for realizada; 0 se ocorrer fim de entrada.
//Pré-condição: texto deve apontar para um buffer com tamanho positivo.
//Pós-condição: o buffer contém a linha lida, sem a quebra de linha.
static int lerLinha(char *texto, size_t tamanho) {
    int c;

    if (fgets(texto, (int)tamanho, stdin) == NULL)
        return 0;

    if (strchr(texto, '\n') == NULL) {
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }

    removerQuebraLinha(texto);
    return 1;
}

//Lê os dados de um produto pelo teclado, exceto o código.
//Entrada: ponteiro para um Produto.
//Retorno: 1 se a leitura for válida; 0 caso contrário.
//Pré-condição: p deve apontar para uma estrutura Produto válida.
//Pós-condição: os campos lidos são preenchidos na estrutura.
int lerProduto(Produto *p) {
    char linha[100];

    if (p == NULL)
        return 0;

    printf("Nome: ");
    if (!lerLinha(p->nome, sizeof(p->nome)))
        return 0;

    printf("Marca: ");
    if (!lerLinha(p->marca, sizeof(p->marca)))
        return 0;

    printf("Categoria: ");
    if (!lerLinha(p->categoria, sizeof(p->categoria)))
        return 0;

    printf("Estoque: ");
    if (!lerLinha(linha, sizeof(linha)) ||
        sscanf(linha, "%d", &p->estoque) != 1)
        return 0;

    printf("Preco (use ponto como separador decimal): ");
    if (!lerLinha(linha, sizeof(linha)) ||
        sscanf(linha, "%f", &p->preco) != 1)
        return 0;

    return validarProduto(p);
}

//Verifica se os dados de um produto respeitam as regras básicas.
//Entrada: ponteiro constante para um Produto.
//Retorno: 1 se os dados forem válidos; 0 caso contrário.
//Pré-condição: p deve apontar para uma estrutura Produto válida.
//Pós-condição: nenhum campo do produto é alterado.
int validarProduto(const Produto *p) {
    if (p == NULL)
        return 0;

    if (p->codigo < 0 || p->estoque < 0 || p->preco < 0.0f)
        return 0;

    if (p->nome[0] == '\0' ||
        p->marca[0] == '\0' ||
        p->categoria[0] == '\0')
        return 0;

    if (memchr(p->nome, '\0', TAM_NOME) == NULL ||
        memchr(p->marca, '\0', TAM_MARCA) == NULL ||
        memchr(p->categoria, '\0', TAM_CATEGORIA) == NULL)
        return 0;

    return 1;
}

//Imprime todos os campos de um produto.
//Entrada: estrutura Produto a ser impressa.
//Retorno: não possui retorno.
//Pré-condição: o produto deve conter campos inicializados.
//Pós-condição: os dados do produto são exibidos na saída padrão.
void imprimirProduto(Produto p) {
    printf("Codigo: %d\n", p.codigo);
    printf("Nome: %s\n", p.nome);
    printf("Marca: %s\n", p.marca);
    printf("Categoria: %s\n", p.categoria);
    printf("Estoque: %d\n", p.estoque);
    printf("Preco: R$ %.2f\n", p.preco);
}

//Atualiza o preço de um produto.
//Entrada: ponteiro para um Produto e o novo preço.
//Retorno: 1 se a atualização for válida; 0 caso contrário.
//Pré-condição: p deve apontar para uma estrutura Produto válida.
//Pós-condição: o preço é atualizado quando o novo valor é válido.
int atualizarPrecoProduto(Produto *p, float novoPreco) {
    if (p == NULL || novoPreco < 0.0f)
        return 0;

    p->preco = novoPreco;
    return 1;
}

//Atualiza o estoque de um produto.
//Entrada: ponteiro para um Produto e o novo estoque.
//Retorno: 1 se a atualização for válida; 0 caso contrário.
//Pré-condição: p deve apontar para uma estrutura Produto válida.
//Pós-condição: o estoque é atualizado quando o novo valor é válido.
int atualizarEstoqueProduto(Produto *p, int novoEstoque) {
    if (p == NULL || novoEstoque < 0)
        return 0;

    p->estoque = novoEstoque;
    return 1;
}
