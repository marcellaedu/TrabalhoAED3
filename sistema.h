#ifndef SISTEMA_H
#define SISTEMA_H

//Cadastra um novo produto no sistema.
//Entrada: Dados do produto informados pelo usuario.
//Retorno: 1 se o cadastro for realizado; 0 caso contrario.
//Pre-condicao: Os arquivos devem estar inicializados.
//Pos-condicao: O produto e gravado nos dados e seu codigo e inserido na Arvore B.

int cadastrarProduto(void);

//Consulta um produto pelo codigo.
//Entrada: Codigo do produto informado pelo usuario.
//Retorno: 1 se a consulta for realizada; 0 caso contrario.
//Pre-condicao: Os arquivos devem estar inicializados.
//Pos-condicao: Os dados do produto encontrado sao exibidos.
 
int consultarProduto(void);

//Atualiza o preco de um produto.
//Entrada: Codigo do produto e novo preco informados pelo usuario.
//Retorno: 1 se a atualizacao for realizada; 0 caso contrario.
//Pre-condicao: Os arquivos devem estar inicializados.
//Pos-condicao: O novo preco e gravado no arquivo de dados.
int atualizarPrecoSistema(void);

//Atualiza o estoque de um produto.
//Entrada: Codigo do produto e novo estoque.
//Retorno: 1 se atualizado; 0 caso contrario.
//Pre-condicao: Os arquivos devem estar inicializados.
//Pos-condicao: O novo estoque e gravado no arquivo.
int atualizarEstoqueSistema(void);

//Exclui um produto do sistema.
//Entrada: Codigo do produto informado pelo usuario.
//Retorno: 1 se excluido; 0 se nao encontrado ou se ocorrer erro.
//Pre-condicao: Os arquivos devem estar inicializados.
//Pos-condicao: O codigo e removido da Arvore B e o registro de dados liberado quando a operacao e concluida.
int excluirProduto(void);

#endif
