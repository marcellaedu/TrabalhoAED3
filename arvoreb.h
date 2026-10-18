#ifndef ARVOREB_H
#define ARVOREB_H

#define ORDEM 5
#define MAX_CHAVES (ORDEM - 1)
#define MIN_CHAVES ((ORDEM - 1) / 2)


/* Estrutura que representa um no da arvore B.
   As posicoes dos filhos referem-se ao arquivo indices.dat.
   As posicoes dos produtos referem-se ao arquivo dados.dat. */
typedef struct NoB {
    int numChaves;
    int chaves[MAX_CHAVES];
    int posDados[MAX_CHAVES];
    int filhos[ORDEM];
    int proxLivre;
} NoB;

//Busca um codigo na arvore B.
//Entrada: Codigo do produto e ponteiro para receber sua posicao nos dados.
//Retorno: 1 se encontrado, 0 se nao encontrado e -1 em caso de erro.
//Pre-condicao: A arvore deve estar inicializada e posDados deve ser valido.
//Pos-condicao: Se encontrado, posDados recebe a posicao do produto.
int buscarArvoreB(int codigo, int *posDados);

//Insere um codigo e sua posicao na arvore B.
//Entrada: Codigo do produto e posicao logica no arquivo de dados.
//Retorno: 1 se inserido, 0 se o codigo ja existir e -1 em caso de erro.
//Pre-condicao: Os arquivos da arvore devem estar inicializados.
//Pos-condicao: O codigo e sua posicao ficam armazenados na arvore B.
int inserirArvoreB(int codigo, int posDados);

//Imprime os nos da Arvore B.
//Entrada: Nao possui parametros.
//Retorno: Nao possui retorno.
//Pre-condicao: Os arquivos de indice devem estar inicializados.
//Pos-condicao: Os nos da arvore sao exibidos na tela.
void imprimirArvoreB(void);

//Lista os codigos da Arvore B em ordem crescente.
//Entrada: Nao possui parametros.
//Retorno: Nao possui retorno.
//Pre-condicao: Os arquivos devem estar inicializados.
//Pos-condicao: Os codigos sao exibidos em ordem crescente.

void listarEmOrdem(void);

//Remove um codigo da Arvore B.
//Entrada: Codigo do produto a ser removido.
//Retorno: 1 se removido, 0 se nao encontrado e -1 em caso de erro.
//Pre-condicao: Os arquivos devem estar inicializados.
//Pos-condicao: O codigo e removido e a estrutura da arvore e ajustada.
int removerArvoreB(int codigo);

#endif
