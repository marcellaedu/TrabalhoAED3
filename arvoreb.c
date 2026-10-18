
#include <stdio.h>
#include "arvoreb.h"
#include "arquivo.h"

/* Estrutura auxiliar utilizada para propagar uma chave
   e seu filho direito durante a insercao. */
typedef struct {
    int codigo;
    int posDados;
    int filhoDireito;
} Promocao;

//Objetivo: Inicializar um no vazio.
//Entrada: Ponteiro para o no.
//Retorno: Nao possui retorno.
//Pre-condicao: no deve apontar para uma estrutura NoB valida.
//Pos-condicao: O no fica vazio, sem filhos e sem elo de livre.
static void inicializarNo(NoB *no){
    int i;

    no->numChaves = 0;
    no->proxLivre = -1;

    for (i = 0; i < MAX_CHAVES; i++) {
        no->chaves[i] = 0;
        no->posDados[i] = -1;
    }

    for (i = 0; i < ORDEM; i++)
        no->filhos[i] = -1;
}

//Objetivo: Encontrar a posicao de uma chave dentro de um no.
//Entrada: No, codigo procurado e ponteiro para receber a posicao.
//Retorno: 1 se encontrado, 0 se ausente e -1 em caso de erro.
//Pre-condicao: no e pos devem ser validos.
//Pos-condicao: pos recebe a posicao encontrada ou de insercao.
static int buscaPos(const NoB *no, int codigo, int *pos){
    if (no == NULL || pos == NULL)
        return -1;

    *pos = 0;

    while (*pos < no->numChaves &&
           codigo > no->chaves[*pos])
        (*pos)++;

    if (*pos < no->numChaves &&
        codigo == no->chaves[*pos])
        return 1;

    return 0;
}

//Objetivo: Verificar se um no e folha.
//Entrada: Ponteiro constante para o no.
//Retorno: 1 se for folha e 0 caso contrario.
//Pre-condicao: no deve apontar para uma estrutura valida.
//Pos-condicao: A estrutura do no permanece inalterada.
static int ehFolha(const NoB *no){
    return no->filhos[0] == -1;
}

//Objetivo: Inserir uma chave e seu filho direito em um no.
//Entrada: No, posicao de insercao, codigo, posicao nos dados e filho direito.
//Retorno: Nao possui retorno.
//Pre-condicao: O no deve possuir espaco para uma chave.
//Pos-condicao: A chave e inserida na posicao correta.
static void adicionaDireita(NoB *no, int pos, int codigo, int posDados, int filhoDireito){
    int i;

    for (i = no->numChaves; i > pos; i--) {
        no->chaves[i] = no->chaves[i - 1];
        no->posDados[i] = no->posDados[i - 1];
    }

    for (i = no->numChaves + 1; i > pos + 1; i--)
        no->filhos[i] = no->filhos[i - 1];

    no->chaves[pos] = codigo;
    no->posDados[pos] = posDados;
    no->filhos[pos + 1] = filhoDireito;
    no->numChaves++;
}

//Divide um no que recebeu uma chave alem do limite.
//Entrada: Posicao do no e vetores temporarios de chaves, dados e filhos.
//Retorno: 1 se a divisao ocorrer e 0 em caso de erro.
//Pre-condicao: Os vetores devem conter ORDEM chaves e ORDEM+1 filhos.
//Pos-condicao: O no original e dividido e a mediana e promovida.
static int split(int posNo, const int chavesTemp[ORDEM], const int dadosTemp[ORDEM], const int filhosTemp[ORDEM + 1], Promocao *promocao){
    NoB esquerda;
    NoB direita;
    int meio;
    int posDireita;
    int i;

    if (posNo < 0 || promocao == NULL)
        return 0;

    meio = ORDEM / 2;

    inicializarNo(&esquerda);
    inicializarNo(&direita);

    esquerda.numChaves = meio;
    direita.numChaves = ORDEM - meio - 1;

    // Copia para o no esquerdo as chaves anteriores a mediana. 
    for (i = 0; i < esquerda.numChaves; i++) {
        esquerda.chaves[i] = chavesTemp[i];
        esquerda.posDados[i] = dadosTemp[i];
    }

    // Copia os filhos do no esquerdo. 
    for (i = 0; i <= esquerda.numChaves; i++)
        esquerda.filhos[i] = filhosTemp[i];

    // Copia para o no direito as chaves posteriores a mediana. 
    for (i = 0; i < direita.numChaves; i++) {
        direita.chaves[i] = chavesTemp[meio + 1 + i];
        direita.posDados[i] = dadosTemp[meio + 1 + i];
    }

    // Copia os filhos do no direito. 
    for (i = 0; i <= direita.numChaves; i++)
        direita.filhos[i] = filhosTemp[meio + 1 + i];

    // Aloca uma nova posicao para o no direito. 
    posDireita = alocarNoIndice();

    if (posDireita == -1)
        return 0;

    if (!gravarNoIndice(posDireita, &direita)) {
        liberarNoIndice(posDireita);
        return 0;
    }

    if (!gravarNoIndice(posNo, &esquerda)) {
        liberarNoIndice(posDireita);
        return 0;
    }

    // A mediana sobe para o no pai. 
    promocao->codigo = chavesTemp[meio];
    promocao->posDados = dadosTemp[meio];
    promocao->filhoDireito = posDireita;

    return 1;
}

//Objetivo: Inserir recursivamente uma chave na arvore B.
//Entrada: Posicao do no, codigo, posicao nos dados e estrutura de promocao.
//Retorno: 1 se houver promocao, 0 sem promocao, -1 em erro e -2 se duplicado.
//Pre-condicao: Os arquivos da arvore devem estar inicializados.
//Pos-condicao: A chave e inserida ou promovida para o no pai.
static int inserirRec(int posNo, int codigo, int posDados, Promocao *promocao){
    NoB no;
    Promocao promoFilho;
    int pos;
    int resultado;
    int i;

    int chavesTemp[ORDEM];
    int dadosTemp[ORDEM];
    int filhosTemp[ORDEM + 1];

    // A posicao -1 indica que foi encontrada uma folha vazia. 
    if (posNo == -1) {
        promocao->codigo = codigo;
        promocao->posDados = posDados;
        promocao->filhoDireito = -1;
        return 1;
    }

    if (!lerNoIndice(posNo, &no))
        return -1;

    resultado = buscaPos(&no, codigo, &pos);

    if (resultado == 1)
        return -2;

    if (resultado == -1)
        return -1;

    if (ehFolha(&no)) {
        promoFilho.codigo = codigo;
        promoFilho.posDados = posDados;
        promoFilho.filhoDireito = -1;
        resultado = 1;
    } else {
        resultado = inserirRec(no.filhos[pos], codigo,
                               posDados, &promoFilho);

        if (resultado <= 0)
            return resultado;
    }

    //Se existe espaco, insere a chave diretamente no no. 
    if (no.numChaves < MAX_CHAVES) {
        adicionaDireita(&no, pos, promoFilho.codigo,
                        promoFilho.posDados,
                        promoFilho.filhoDireito);

        return gravarNoIndice(posNo, &no) ? 0 : -1;
    }

    //Monta temporariamente as ORDEM chaves, incluindo a nova. 
    for (i = 0; i < pos; i++) {
        chavesTemp[i] = no.chaves[i];
        dadosTemp[i] = no.posDados[i];
    }

    chavesTemp[pos] = promoFilho.codigo;
    dadosTemp[pos] = promoFilho.posDados;

    for (i = pos; i < MAX_CHAVES; i++) {
        chavesTemp[i + 1] = no.chaves[i];
        dadosTemp[i + 1] = no.posDados[i];
    }

    // Insere temporariamente o filho direito da chave promovida. 
    for (i = 0; i <= pos; i++)
        filhosTemp[i] = no.filhos[i];

    filhosTemp[pos + 1] = promoFilho.filhoDireito;

    // Copia o restante dos filhos a partir de pos
    for (i = pos + 1; i <= no.numChaves; i++)
        filhosTemp[i + 1] = no.filhos[i];

    //Divide o no e devolve a mediana para o nivel superior. 
    if (!split(posNo, chavesTemp, dadosTemp,
               filhosTemp, promocao))
        return -1;

    return 1;
}

//Busca um codigo na arvore B.
//Entrada: Codigo e ponteiro para receber a posicao nos dados.
//Retorno: 1 se encontrado, 0 se ausente e -1 em caso de erro.
//Pre-condicao: A arvore deve estar inicializada e posDados deve ser valido.
//Pos-condicao: A posicao do produto e retornada quando encontrado.
int buscarArvoreB(int codigo, int *posDados)
{
    CabecalhoIndices cab;
    NoB no;
    int posNo;
    int pos;
    int resultado;

    if (codigo <= 0 || posDados == NULL)
        return -1;

    if (!lerCabecalhoIndices(&cab))
        return -1;

    posNo = cab.raiz;

    while (posNo != -1) {
        if (!lerNoIndice(posNo, &no))
            return -1;

        resultado = buscaPos(&no, codigo, &pos);

        if (resultado == 1) {
            *posDados = no.posDados[pos];
            return 1;
        }

        if (resultado == -1)
            return -1;

        posNo = no.filhos[pos];
    }

    return 0;
}

//Insere um codigo e sua posicao na arvore B.
//Entrada: Codigo do produto e posicao logica no arquivo de dados.
//Retorno: 1 se inserido, 0 se duplicado e -1 em caso de erro.
//Pre-condicao: Os arquivos devem estar inicializados.
//Pos-condicao: O codigo e sua posicao ficam armazenados na arvore B.
int inserirArvoreB(int codigo, int posDados)
{
    CabecalhoIndices cab;
    NoB raiz;
    Promocao promocao;
    int resultado;
    int posRaiz;

    if (codigo <= 0 || posDados < 0)
        return -1;

    if (!lerCabecalhoIndices(&cab))
        return -1;

    if (cab.raiz == -1) {
        posRaiz = alocarNoIndice();

        if (posRaiz == -1)
            return -1;

        inicializarNo(&raiz);
        raiz.numChaves = 1;
        raiz.chaves[0] = codigo;
        raiz.posDados[0] = posDados;

        if (!gravarNoIndice(posRaiz, &raiz)) {
            liberarNoIndice(posRaiz);
            return -1;
        }

        /* Relê o cabeçalho para preservar o topo atualizado. */
        if (!lerCabecalhoIndices(&cab))
            return -1;

        cab.raiz = posRaiz;

        if (!gravarCabecalhoIndices(&cab))
            return -1;

        return 1;
    }

    resultado = inserirRec(cab.raiz, codigo, posDados, &promocao);

    if (resultado == -2)
        return 0;

    if (resultado == -1)
        return -1;

    if (resultado == 0)
        return 1;

    inicializarNo(&raiz);
    raiz.numChaves = 1;
    raiz.chaves[0] = promocao.codigo;
    raiz.posDados[0] = promocao.posDados;
    raiz.filhos[0] = cab.raiz;
    raiz.filhos[1] = promocao.filhoDireito;

    posRaiz = alocarNoIndice();

    if (posRaiz == -1)
        return -1;

    if (!gravarNoIndice(posRaiz, &raiz)) {
        liberarNoIndice(posRaiz);
        return -1;
    }

    // Relê o cabeçalho para preservar o topo atualizado. 
    if (!lerCabecalhoIndices(&cab))
        return -1;

    cab.raiz = posRaiz;

    if (!gravarCabecalhoIndices(&cab))
        return -1;

    return 1;
}

//Remove uma chave de um no.
//Entrada: No e posicao da chave.
//Retorno: Nao possui retorno.
//Pre-condicao: A posicao deve identificar uma chave existente.
//Pos-condicao: As chaves posteriores sao deslocadas para a esquerda.
static void removerChaveNo(NoB *no, int pos)
{
    int i;

    for (i = pos; i < no->numChaves - 1; i++) {
        no->chaves[i] = no->chaves[i + 1];
        no->posDados[i] = no->posDados[i + 1];
    }

    no->numChaves--;
    no->chaves[no->numChaves] = 0;
    no->posDados[no->numChaves] = -1;
}

//Remove uma chave e seu filho direito do pai.
//Entrada: No pai e indice da chave.
//Retorno: Nao possui retorno.
//Pre-condicao: O indice deve identificar uma chave existente.
//Pos-condicao: A chave e o filho direito sao removidos do pai.
static void removerChavePai(NoB *pai, int indice)
{
    int i;

    for (i = indice; i < pai->numChaves - 1; i++) {
        pai->chaves[i] = pai->chaves[i + 1];
        pai->posDados[i] = pai->posDados[i + 1];
    }

    for (i = indice + 1; i < pai->numChaves; i++)
        pai->filhos[i] = pai->filhos[i + 1];

    pai->numChaves--;
    pai->chaves[pai->numChaves] = 0;
    pai->posDados[pai->numChaves] = -1;
    pai->filhos[pai->numChaves + 1] = -1;
}

//Corrige o underflow por redistribuicao ou concatenacao.
//Entrada: No pai e indice do filho com underflow.
//Retorno: 1 se corrigido e 0 em caso de erro.
//Pre-condicao: O filho indicado possui menos que MIN_CHAVES.
//Pos-condicao: O filho e ajustado com seu irmao adjacente.
static int corrigirUnderflow(NoB *pai, int indice)
{
    NoB filho, irmao;
    int posFilho = pai->filhos[indice];
    int posIrmao;
    int i, total;

    if (indice > 0) {
        posIrmao = pai->filhos[indice - 1];

        if (!lerNoIndice(posFilho, &filho) ||
            !lerNoIndice(posIrmao, &irmao))
            return 0;

        //Redistribuicao com o irmao esquerdo.
        if (irmao.numChaves > MIN_CHAVES) {
            for (i = filho.numChaves; i > 0; i--) {
                filho.chaves[i] = filho.chaves[i - 1];
                filho.posDados[i] = filho.posDados[i - 1];
            }

            for (i = filho.numChaves + 1; i > 0; i--)
                filho.filhos[i] = filho.filhos[i - 1];

            filho.chaves[0] = pai->chaves[indice - 1];
            filho.posDados[0] = pai->posDados[indice - 1];
            filho.filhos[0] = irmao.filhos[irmao.numChaves];

            pai->chaves[indice - 1] =
                irmao.chaves[irmao.numChaves - 1];
            pai->posDados[indice - 1] =
                irmao.posDados[irmao.numChaves - 1];

            irmao.numChaves--;
            filho.numChaves++;

            irmao.chaves[irmao.numChaves] = 0;
            irmao.posDados[irmao.numChaves] = -1;
            irmao.filhos[irmao.numChaves + 1] = -1;

            return gravarNoIndice(posIrmao, &irmao) &&
                   gravarNoIndice(posFilho, &filho);
        }
    }

    if (indice < pai->numChaves) {
        posIrmao = pai->filhos[indice + 1];

        if (!lerNoIndice(posFilho, &filho) ||
            !lerNoIndice(posIrmao, &irmao))
            return 0;

        //Redistribuicao com o irmao direito.
        if (irmao.numChaves > MIN_CHAVES) {
            filho.chaves[filho.numChaves] = pai->chaves[indice];
            filho.posDados[filho.numChaves] = pai->posDados[indice];
            filho.filhos[filho.numChaves + 1] = irmao.filhos[0];

            pai->chaves[indice] = irmao.chaves[0];
            pai->posDados[indice] = irmao.posDados[0];

            filho.numChaves++;

            for (i = 0; i < irmao.numChaves - 1; i++) {
                irmao.chaves[i] = irmao.chaves[i + 1];
                irmao.posDados[i] = irmao.posDados[i + 1];
            }

            for (i = 0; i < irmao.numChaves; i++)
                irmao.filhos[i] = irmao.filhos[i + 1];

            irmao.numChaves--;
            irmao.chaves[irmao.numChaves] = 0;
            irmao.posDados[irmao.numChaves] = -1;
            irmao.filhos[irmao.numChaves + 1] = -1;

            return gravarNoIndice(posIrmao, &irmao) &&
                   gravarNoIndice(posFilho, &filho);
        }
    }

    //Concatenacao com o irmao esquerdo, quando possivel.
    if (indice > 0) {
        posIrmao = pai->filhos[indice - 1];

        if (!lerNoIndice(posIrmao, &irmao) ||
            !lerNoIndice(posFilho, &filho))
            return 0;

        total = irmao.numChaves + 1 + filho.numChaves;

        if (total > MAX_CHAVES)
            return 0;

        irmao.chaves[irmao.numChaves] = pai->chaves[indice - 1];
        irmao.posDados[irmao.numChaves] = pai->posDados[indice - 1];

        for (i = 0; i < filho.numChaves; i++) {
            irmao.chaves[irmao.numChaves + 1 + i] = filho.chaves[i];
            irmao.posDados[irmao.numChaves + 1 + i] =
                filho.posDados[i];
        }

        for (i = 0; i <= filho.numChaves; i++)
            irmao.filhos[irmao.numChaves + 1 + i] =
                filho.filhos[i];

        irmao.numChaves = total;

        if (!gravarNoIndice(posIrmao, &irmao))
            return 0;

        removerChavePai(pai, indice - 1);

        if (!liberarNoIndice(posFilho))
            return 0;

        return 1;
    }

    //Concatenacao com o irmao direito.
    if (indice < pai->numChaves) {
        posIrmao = pai->filhos[indice + 1];

        if (!lerNoIndice(posFilho, &filho) ||
            !lerNoIndice(posIrmao, &irmao))
            return 0;

        total = filho.numChaves + 1 + irmao.numChaves;

        if (total > MAX_CHAVES)
            return 0;

        filho.chaves[filho.numChaves] = pai->chaves[indice];
        filho.posDados[filho.numChaves] = pai->posDados[indice];

        for (i = 0; i < irmao.numChaves; i++) {
            filho.chaves[filho.numChaves + 1 + i] = irmao.chaves[i];
            filho.posDados[filho.numChaves + 1 + i] =
                irmao.posDados[i];
        }

        for (i = 0; i <= irmao.numChaves; i++)
            filho.filhos[filho.numChaves + 1 + i] =
                irmao.filhos[i];

        filho.numChaves = total;

        if (!gravarNoIndice(posFilho, &filho))
            return 0;

        removerChavePai(pai, indice);

        if (!liberarNoIndice(posIrmao))
            return 0;

        return 1;
    }

    return 0;
}

//Remove recursivamente uma chave da Arvore B.
//Entrada: Posicao do no, codigo e ponteiro para indicar underflow.
//Retorno: 1 se removido, 0 se ausente e -1 em caso de erro.
//Pre-condicao: A arvore deve estar inicializada.
//Pos-condicao: A chave e removida e os nos sao ajustados.
static int removerRec(int posNo, int codigo, int *underflow)
{
    NoB no, aux;
    int pos, resultado;
    int posAntecessor, codigoAntecessor, dadosAntecessor;
    int underflowFilho;

    if (posNo == -1)
        return 0;

    if (!lerNoIndice(posNo, &no))
        return -1;

    resultado = buscaPos(&no, codigo, &pos);

    if (resultado == -1)
        return -1;

    //Chave encontrada.
    if (resultado == 1) {
        if (ehFolha(&no)) {
            removerChaveNo(&no, pos);

            if (!gravarNoIndice(posNo, &no))
                return -1;

            *underflow = no.numChaves < MIN_CHAVES;
            return 1;
        }

        //Substitui a chave pelo antecessor.
        posAntecessor = no.filhos[pos];

        while (1) {
            if (!lerNoIndice(posAntecessor, &aux))
                return -1;

            if (ehFolha(&aux))
                break;

            posAntecessor = aux.filhos[aux.numChaves];
        }

        codigoAntecessor = aux.chaves[aux.numChaves - 1];
        dadosAntecessor = aux.posDados[aux.numChaves - 1];

        // Remove o antecessor antes de substituir a chave do no atual.
        resultado = removerRec(no.filhos[pos], codigoAntecessor, &underflowFilho);

        if (resultado != 1)
            return resultado;

        no.chaves[pos] = codigoAntecessor;
        no.posDados[pos] = dadosAntecessor;

        if (underflowFilho &&
            !corrigirUnderflow(&no, pos))
            return -1;

        if (!gravarNoIndice(posNo, &no))
            return -1;

        *underflow = no.numChaves < MIN_CHAVES;
        return 1;
    }

    //Chave nao encontrada: continua no filho adequado.
    if (no.filhos[pos] == -1)
        return 0;

    resultado = removerRec(no.filhos[pos], codigo, &underflowFilho);

    if (resultado != 1)
        return resultado;

    if (underflowFilho &&
        !corrigirUnderflow(&no, pos))
        return -1;

    if (!gravarNoIndice(posNo, &no))
        return -1;

    *underflow = no.numChaves < MIN_CHAVES;
    return 1;
}

//Remove um codigo da Arvore B.
//Entrada: Codigo do produto a ser removido.
//Retorno: 1 se removido, 0 se nao encontrado e -1 em caso de erro.
//Pre-condicao: Os arquivos devem estar inicializados.
//Pos-condicao: O codigo e removido e a raiz e ajustada quando necessario.
int removerArvoreB(int codigo)
{
    CabecalhoIndices cab;
    NoB raiz;
    int underflow;
    int resultado;
    int novaRaiz;

    if (codigo <= 0)
        return -1;

    if (!lerCabecalhoIndices(&cab))
        return -1;

    if (cab.raiz == -1)
        return 0;

    resultado = removerRec(cab.raiz, codigo, &underflow);

    if (resultado != 1)
        return resultado;

    if (!lerNoIndice(cab.raiz, &raiz))
        return -1;

    //Se a raiz ficar vazia, seu unico filho assume a raiz.
    if (raiz.numChaves == 0) {
        novaRaiz = raiz.filhos[0];

        if (!liberarNoIndice(cab.raiz))
            return -1;

        if (!lerCabecalhoIndices(&cab))
            return -1;

        cab.raiz = novaRaiz;

        if (!gravarCabecalhoIndices(&cab))
            return -1;
    }

    return 1;
}

 //Objetivo: Imprimir os nos de um nivel da Arvore B.
 //Entrada: Posicao logica do no e nivel de profundidade.
 //Retorno: Quantidade de nos encontrados no nivel.
 //Pre-condicao: A posicao deve identificar um no valido ou ser -1.
 //Pos-condicao: Os nos do nivel desejado sao exibidos na tela.
static int imprimirNivel(int posicao, int nivelAtual, int nivelDesejado){
    NoB no;
    int quantidade = 0;

    if (posicao == -1)
        return 0;

    if (!lerNoIndice(posicao, &no))
        return 0;

    if (nivelAtual == nivelDesejado){
        printf("[");

        for (int i = 0; i < no.numChaves; i++)
        {
            printf("%d", no.chaves[i]);

            if (i < no.numChaves - 1)
                printf(" | ");
        }

        printf("] ");
        return 1;
    }

    for (int i = 0; i <= no.numChaves; i++){
        quantidade += imprimirNivel(no.filhos[i],nivelAtual + 1, nivelDesejado);
    }
    return quantidade;
}


 //Objetivo: Imprimir a Arvore B armazenada no arquivo de indices.
 //Entrada: Nao possui parametros.
 //Retorno: Nao possui retorno.
 //Pre-condicao: Os arquivos devem estar inicializados.
 //Pos-condicao: A estrutura da arvore e exibida na tela.
void imprimirArvoreB(void)
{
    CabecalhoIndices cab;
    int nivel = 0;
    int encontrou;

    if (!lerCabecalhoIndices(&cab))
    {
        printf("Erro ao ler o arquivo de indices.\n");
        return;
    }

    if (cab.raiz == -1)
    {
        printf("Arvore B vazia.\n");
        return;
    }

    while (nivel < 100)
    {
        encontrou = imprimirNivel(cab.raiz, 0, nivel);

        if (encontrou == 0)
            break;

        printf("\n");
        nivel++;
    }
}

//Percorre a Arvore B em ordem crescente.
//Entrada: Posicao logica do no.
//Retorno: Nao possui retorno.
//Pre-condicao: A arvore deve estar inicializada.
//Pos-condicao: Os codigos sao exibidos em ordem crescente.
static void listar(int posicao){
    NoB no;
    RegistroDados registro;
    int i;

    if (posicao == -1)
        return;

    if (!lerNoIndice(posicao, &no)) {
        printf("Erro ao ler o no.\n");
        return;
    }

    for (i = 0; i < no.numChaves; i++) {
            listar(no.filhos[i]);

            if (lerRegistroDados(no.posDados[i], &registro)) {
                printf(" %d - %s\n",
                    no.chaves[i], registro.produto.nome);
            } else {
                printf("Erro ao ler produto %d.\n", no.chaves[i]);
            }
        }

    listar(no.filhos[no.numChaves]);
}

//Lista os codigos da Arvore B em ordem crescente.
//Entrada: Nao possui parametros.
//Retorno: Nao possui retorno.
//Pre-condicao: Os arquivos devem estar inicializados.
//Pos-condicao: Os codigos sao exibidos em ordem crescente.

void listarEmOrdem(void)
{
    CabecalhoIndices cab;

    if (!lerCabecalhoIndices(&cab)) {
        printf("Erro ao ler os indices.\n");
        return;
    }

    if (cab.raiz == -1) {
        printf("A Arvore B esta vazia.\n");
        return;
    }

    printf("\n===== PRODUTOS EM ORDEM =====\n");
    listar(cab.raiz);
}

