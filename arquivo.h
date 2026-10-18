#ifndef ARQUIVO_H
#define ARQUIVO_H

#include "produto.h"
#include "arvoreb.h"

#define ARQUIVO_DADOS "dados.dat"
#define ARQUIVO_INDICES "indices.dat"

typedef struct {
    int topo;
    int livres;
} CabecalhoDados;

typedef struct {
    int raiz;
    int topo;
    int livres;
} CabecalhoIndices;

typedef struct {
    Produto produto;
    int proxLivre;
} RegistroDados;

//Cria os arquivos binários e inicializa seus cabeçalhos, caso necessário.
//Entrada: não possui parâmetros.
//Retorno: 1 se os arquivos estiverem prontos; 0 se ocorrer erro.
//Pré-condição: os arquivos devem estar acessíveis para leitura e gravação.
//Pós-condição: os dois arquivos existem e possuem cabeçalhos válidos.
int inicializarArquivos(void);

//Lê o cabeçalho do arquivo de dados.
//Entrada: ponteiro para receber o cabeçalho.
//Retorno: 1 se a leitura for realizada; 0 caso contrário.
//Pré-condição: cab deve apontar para uma estrutura CabecalhoDados válida.
//Pós-condição: cab contém o cabeçalho lido do arquivo.
int lerCabecalhoDados(CabecalhoDados *cab);

//Grava o cabeçalho do arquivo de dados.
//Entrada: ponteiro constante para o cabeçalho.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: cab deve apontar para uma estrutura CabecalhoDados válida.
//Pós-condição: o cabeçalho do arquivo de dados é atualizado.
int gravarCabecalhoDados(const CabecalhoDados *cab);

//Lê o cabeçalho do arquivo de índices.
//Entrada: ponteiro para receber o cabeçalho.
//Retorno: 1 se a leitura for realizada; 0 caso contrário.
//Pré-condição: cab deve apontar para uma estrutura CabecalhoIndices válida.
//Pós-condição: cab contém o cabeçalho lido do arquivo.
int lerCabecalhoIndices(CabecalhoIndices *cab);

//Grava o cabeçalho do arquivo de índices.
//Entrada: ponteiro constante para o cabeçalho.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: cab deve apontar para uma estrutura CabecalhoIndices válida.
//Pós-condição: o cabeçalho do arquivo de índices é atualizado.
int gravarCabecalhoIndices(const CabecalhoIndices *cab);

//Lê um registro de dados pela posição lógica.
//Entrada: posição lógica do registro e ponteiro para receber os dados.
//Retorno: 1 se a leitura for realizada; 0 caso contrário.
//Pré-condição: posicao deve ser não negativa e registro não deve ser nulo.
//Pós-condição: registro contém o produto e o elo de livre lidos.
int lerRegistroDados(int posicao, RegistroDados *registro);

//Grava um registro de dados em uma posição lógica.
//Entrada: posição lógica e ponteiro constante para o registro.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: posicao deve ser não negativa e registro não deve ser nulo.
//Pós-condição: o registro é gravado na posição informada.
int gravarRegistroDados(int posicao, const RegistroDados *registro);

//Obtém uma posição para um novo registro de dados, reutilizando posições livres.
//Entrada: não possui parâmetros.
//Retorno: posição lógica disponível ou -1 em caso de erro.
//Pré-condição: o arquivo de dados deve estar inicializado.
//Pós-condição: uma posição é retirada da lista de livres ou o topo é avançado.
int alocarRegistroDados(void);

//Devolve uma posição de dados à lista de registros livres.
//Entrada: posição lógica a liberar.
//Retorno: 1 se a posição for liberada; 0 caso contrário.
//Pré-condição: posicao deve identificar um registro existente e ocupado.
//Pós-condição: a posição passa a ser a cabeça da lista de livres.
int liberarRegistroDados(int posicao);

//Lê um nó da árvore B pela posição lógica.
//Entrada: posição lógica do nó e ponteiro para receber o nó.
//Retorno: 1 se a leitura for realizada; 0 caso contrário.
//Pré-condição: posicao deve ser não negativa e no deve ser válido.
//Pós-condição: no contém o nó lido do arquivo de índices.
int lerNoIndice(int posicao, NoB *no);

//Grava um nó da árvore B em uma posição lógica.
//Entrada: posição lógica e ponteiro constante para o nó.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: posicao deve ser não negativa e no deve ser válido.
//Pós-condição: o nó é gravado na posição informada.
int gravarNoIndice(int posicao, const NoB *no);

//Obtém uma posição para um novo nó da árvore B, reutilizando posições livres.
//Entrada: não possui parâmetros.
//Retorno: posição lógica disponível ou -1 em caso de erro.
//Pré-condição: o arquivo de índices deve estar inicializado.
//Pós-condição: uma posição é retirada da lista de livres ou o topo é avançado.
int alocarNoIndice(void);

//Devolve uma posição de nó à lista de nós livres.
//Entrada: posição lógica a liberar.
//Retorno: 1 se a posição for liberada; 0 caso contrário.
//Pré-condição: posicao deve identificar um nó existente e ocupado.
//Pós-condição: a posição passa a ser a cabeça da lista de nós livres.
int liberarNoIndice(int posicao);

#endif
