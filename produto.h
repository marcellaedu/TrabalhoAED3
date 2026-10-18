#ifndef PRODUTO_H
#define PRODUTO_H

#define TAM_NOME 51
#define TAM_MARCA 31
#define TAM_CATEGORIA 51

typedef struct {
    int codigo;
    char nome[TAM_NOME];
    char marca[TAM_MARCA];
    char categoria[TAM_CATEGORIA];
    int estoque;
    float preco;
} Produto;

//Lê os dados de um produto pelo teclado, exceto o código.
//Entrada: ponteiro para um Produto.
//Retorno: 1 se a leitura for válida; 0 caso contrário.
//Pré-condição: p deve apontar para uma estrutura Produto válida.
//Pós-condição: os campos lidos são preenchidos na estrutura.
int lerProduto(Produto *p);

//Verifica se os dados de um produto respeitam as regras básicas.
//Entrada: ponteiro constante para um Produto.
//Retorno: 1 se os dados forem válidos; 0 caso contrário.
//Pré-condição: p deve apontar para uma estrutura Produto válida.
//Pós-condição: nenhum campo do produto é alterado.
int validarProduto(const Produto *p);

//Imprime todos os campos de um produto.
//Entrada: estrutura Produto a ser impressa.
//Retorno: não possui retorno.
//Pré-condição: o produto deve conter campos inicializados.
//Pós-condição: os dados do produto são exibidos na saída padrão.
void imprimirProduto(Produto p);

//Atualiza o preço de um produto.
//Entrada: ponteiro para um Produto e o novo preço.
//Retorno: 1 se a atualização for válida; 0 caso contrário.
//Pré-condição: p deve apontar para uma estrutura Produto válida.
//Pós-condição: o preço é atualizado quando o novo valor é válido.
int atualizarPrecoProduto(Produto *p, float novoPreco);

//Atualiza o estoque de um produto.
//Entrada: ponteiro para um Produto e o novo estoque.
//Retorno: 1 se a atualização for válida; 0 caso contrário.
//Pré-condição: p deve apontar para uma estrutura Produto válida.
//Pós-condição: o estoque é atualizado quando o novo valor é válido.
int atualizarEstoqueProduto(Produto *p, int novoEstoque);

#endif
