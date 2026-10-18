#include <stdio.h>
#include <string.h>
#include "arquivo.h"

//Calcula a posição física de um registro no arquivo de dados.
//Entrada: posição lógica do registro.
//Retorno: deslocamento em bytes desde o início do arquivo.
//Pré-condição: posicao deve ser não negativa.
//Pós-condição: retorna o deslocamento correspondente ao registro.
static long deslocamentoDados(int posicao) {
    return (long)sizeof(CabecalhoDados) +
           (long)posicao * (long)sizeof(RegistroDados);
}

//Calcula a posição física de um nó no arquivo de índices.
//Entrada: posição lógica do nó.
//Retorno: deslocamento em bytes desde o início do arquivo.
//Pré-condição: posicao deve ser não negativa.
//Pós-condição: retorna o deslocamento correspondente ao nó.
static long deslocamentoIndice(int posicao) {
    return (long)sizeof(CabecalhoIndices) +
           (long)posicao * (long)sizeof(NoB);
}

//Lê o cabeçalho a partir de um arquivo já aberto.
//Entrada: arquivo aberto e ponteiro para receber o cabeçalho.
//Retorno: 1 se a leitura for realizada; 0 caso contrário.
//Pré-condição: arq e cab devem ser válidos.
//Pós-condição: cab recebe os dados do cabeçalho.
static int lerCabDadosAberto(FILE *arq, CabecalhoDados *cab) {
    if (arq == NULL || cab == NULL)
        return 0;
    if (fseek(arq, 0L, SEEK_SET) != 0)
        return 0;
    return fread(cab, sizeof(*cab), 1, arq) == 1;
}

//Grava o cabeçalho em um arquivo já aberto.
//Entrada: arquivo aberto e ponteiro constante para o cabeçalho.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: arq e cab devem ser válidos.
//Pós-condição: o cabeçalho é escrito no início do arquivo.
static int gravarCabDadosAberto(FILE *arq, const CabecalhoDados *cab) {
    if (arq == NULL || cab == NULL)
        return 0;
    if (fseek(arq, 0L, SEEK_SET) != 0)
        return 0;
    return fwrite(cab, sizeof(*cab), 1, arq) == 1;
}

//Lê o cabeçalho de índices a partir de um arquivo já aberto.
//Entrada: arquivo aberto e ponteiro para receber o cabeçalho.
//Retorno: 1 se a leitura for realizada; 0 caso contrário.
//Pré-condição: arq e cab devem ser válidos.
//Pós-condição: cab recebe os dados do cabeçalho.
static int lerCabIndicesAberto(FILE *arq, CabecalhoIndices *cab) {
    if (arq == NULL || cab == NULL)
        return 0;
    if (fseek(arq, 0L, SEEK_SET) != 0)
        return 0;
    return fread(cab, sizeof(*cab), 1, arq) == 1;
}

//Grava o cabeçalho de índices em um arquivo já aberto.
//Entrada: arquivo aberto e ponteiro constante para o cabeçalho.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: arq e cab devem ser válidos.
//Pós-condição: o cabeçalho é escrito no início do arquivo.
static int gravarCabIndicesAberto(FILE *arq, const CabecalhoIndices *cab) {
    if (arq == NULL || cab == NULL)
        return 0;
    if (fseek(arq, 0L, SEEK_SET) != 0)
        return 0;
    return fwrite(cab, sizeof(*cab), 1, arq) == 1;
}

//Inicializa um arquivo binário vazio com um cabeçalho.
//Entrada: nome do arquivo e endereço do cabeçalho inicial.
//Retorno: 1 se o arquivo for inicializado; 0 caso contrário.
//Pré-condição: nome e cab devem ser válidos.
//Pós-condição: o arquivo contém o cabeçalho inicial.
static int criarArquivoDados(const char *nome, const CabecalhoDados *cab) {
    FILE *arq = fopen(nome, "rb");
    if (arq != NULL) {
        fclose(arq);
        return 1;
    }

    arq = fopen(nome, "wb");
    if (arq == NULL)
        return 0;

    int ok = fwrite(cab, sizeof(*cab), 1, arq) == 1;
    if (fclose(arq) != 0)
        ok = 0;
    return ok;
}

//Inicializa um arquivo de índices vazio com um cabeçalho.
//Entrada: nome do arquivo e endereço do cabeçalho inicial.
//Retorno: 1 se o arquivo for inicializado; 0 caso contrário.
//Pré-condição: nome e cab devem ser válidos.
//Pós-condição: o arquivo contém o cabeçalho inicial.
static int criarArquivoIndices(const char *nome, const CabecalhoIndices *cab) {
    FILE *arq = fopen(nome, "rb");
    if (arq != NULL) {
        fclose(arq);
        return 1;
    }

    arq = fopen(nome, "wb");
    if (arq == NULL)
        return 0;

    int ok = fwrite(cab, sizeof(*cab), 1, arq) == 1;
    if (fclose(arq) != 0)
        ok = 0;
    return ok;
}

//Cria os arquivos binários e inicializa seus cabeçalhos, caso necessário.
//Entrada: não possui parâmetros.
//Retorno: 1 se os arquivos estiverem prontos; 0 se ocorrer erro.
//Pré-condição: os arquivos devem estar acessíveis para leitura e gravação.
//Pós-condição: os dois arquivos existem e possuem cabeçalhos válidos.
int inicializarArquivos(void) {
    CabecalhoDados cabDados;
    CabecalhoIndices cabIndices;

    cabDados.topo = 0;
    cabDados.livres = -1;

    cabIndices.raiz = -1;
    cabIndices.topo = 0;
    cabIndices.livres = -1;

    if (!criarArquivoDados(ARQUIVO_DADOS, &cabDados))
        return 0;
    if (!criarArquivoIndices(ARQUIVO_INDICES, &cabIndices))
        return 0;

    return 1;
}

//Lê o cabeçalho do arquivo de dados.
//Entrada: ponteiro para receber o cabeçalho.
//Retorno: 1 se a leitura for realizada; 0 caso contrário.
//Pré-condição: cab deve apontar para uma estrutura CabecalhoDados válida.
//Pós-condição: cab contém o cabeçalho lido do arquivo.
int lerCabecalhoDados(CabecalhoDados *cab) {
    FILE *arq;
    int ok;

    if (cab == NULL)
        return 0;

    arq = fopen(ARQUIVO_DADOS, "rb");
    if (arq == NULL)
        return 0;

    ok = lerCabDadosAberto(arq, cab);
    fclose(arq);
    return ok;
}

//Grava o cabeçalho do arquivo de dados.
//Entrada: ponteiro constante para o cabeçalho.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: cab deve apontar para uma estrutura CabecalhoDados válida.
//Pós-condição: o cabeçalho do arquivo de dados é atualizado.
int gravarCabecalhoDados(const CabecalhoDados *cab) {
    FILE *arq;
    int ok;

    if (cab == NULL)
        return 0;

    arq = fopen(ARQUIVO_DADOS, "rb+");
    if (arq == NULL)
        return 0;

    ok = gravarCabDadosAberto(arq, cab);
    if (fclose(arq) != 0)
        ok = 0;
    return ok;
}

//Lê o cabeçalho do arquivo de índices.
//Entrada: ponteiro para receber o cabeçalho.
//Retorno: 1 se a leitura for realizada; 0 caso contrário.
//Pré-condição: cab deve apontar para uma estrutura CabecalhoIndices válida.
//Pós-condição: cab contém o cabeçalho lido do arquivo.
int lerCabecalhoIndices(CabecalhoIndices *cab) {
    FILE *arq;
    int ok;

    if (cab == NULL)
        return 0;

    arq = fopen(ARQUIVO_INDICES, "rb");
    if (arq == NULL)
        return 0;

    ok = lerCabIndicesAberto(arq, cab);
    fclose(arq);
    return ok;
}

//Grava o cabeçalho do arquivo de índices.
//Entrada: ponteiro constante para o cabeçalho.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: cab deve apontar para uma estrutura CabecalhoIndices válida.
//Pós-condição: o cabeçalho do arquivo de índices é atualizado.
int gravarCabecalhoIndices(const CabecalhoIndices *cab) {
    FILE *arq;
    int ok;

    if (cab == NULL)
        return 0;

    arq = fopen(ARQUIVO_INDICES, "rb+");
    if (arq == NULL)
        return 0;

    ok = gravarCabIndicesAberto(arq, cab);
    if (fclose(arq) != 0)
        ok = 0;
    return ok;
}

//Lê um registro de dados pela posição lógica.
//Entrada: posição lógica do registro e ponteiro para receber os dados.
//Retorno: 1 se a leitura for realizada; 0 caso contrário.
//Pré-condição: posicao deve ser não negativa e registro não deve ser nulo.
//Pós-condição: registro contém o produto e o elo de livre lidos.
int lerRegistroDados(int posicao, RegistroDados *registro) {
    FILE *arq;
    CabecalhoDados cab;
    int ok;

    if (posicao < 0 || registro == NULL)
        return 0;

    arq = fopen(ARQUIVO_DADOS, "rb");
    if (arq == NULL)
        return 0;

    if (!lerCabDadosAberto(arq, &cab) || posicao >= cab.topo) {
        fclose(arq);
        return 0;
    }

    ok = fseek(arq, deslocamentoDados(posicao), SEEK_SET) == 0 &&
         fread(registro, sizeof(*registro), 1, arq) == 1;
    fclose(arq);
    return ok;
}

//Grava um registro de dados em uma posição lógica.
//Entrada: posição lógica e ponteiro constante para o registro.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: posicao deve ser não negativa e registro não deve ser nulo.
//Pós-condição: o registro é gravado na posição informada.
int gravarRegistroDados(int posicao, const RegistroDados *registro) {
    FILE *arq;
    CabecalhoDados cab;
    int ok;

    if (posicao < 0 || registro == NULL)
        return 0;

    arq = fopen(ARQUIVO_DADOS, "rb+");
    if (arq == NULL)
        return 0;

    if (!lerCabDadosAberto(arq, &cab) || posicao > cab.topo) {
        fclose(arq);
        return 0;
    }

    ok = fseek(arq, deslocamentoDados(posicao), SEEK_SET) == 0 &&
         fwrite(registro, sizeof(*registro), 1, arq) == 1;

    if (ok && posicao == cab.topo) {
        cab.topo++;
        ok = gravarCabDadosAberto(arq, &cab);
    }

    if (fclose(arq) != 0)
        ok = 0;
    return ok;
}

//Obtém uma posição para um novo registro de dados, reutilizando posições livres.
//Entrada: não possui parâmetros.
//Retorno: posição lógica disponível ou -1 em caso de erro.
//Pré-condição: o arquivo de dados deve estar inicializado.
//Pós-condição: uma posição é retirada da lista de livres ou o topo é avançado.
int alocarRegistroDados(void) {
    FILE *arq = fopen(ARQUIVO_DADOS, "rb+");
    CabecalhoDados cab;
    RegistroDados reg;
    int pos;

    if (arq == NULL)
        return -1;

    if (!lerCabDadosAberto(arq, &cab)) {
        fclose(arq);
        return -1;
    }

    if (cab.livres != -1) {
        pos = cab.livres;
        if (pos < 0 || pos >= cab.topo ||
            fseek(arq, deslocamentoDados(pos), SEEK_SET) != 0 ||
            fread(&reg, sizeof(reg), 1, arq) != 1) {
            fclose(arq);
            return -1;
        }
        cab.livres = reg.proxLivre;
    } else {
        pos = cab.topo;
        cab.topo++;
    }

    if (!gravarCabDadosAberto(arq, &cab)) {
        fclose(arq);
        return -1;
    }

    if (fclose(arq) != 0)
        return -1;
    return pos;
}

//Devolve uma posição de dados à lista de registros livres.
//Entrada: posição lógica a liberar.
//Retorno: 1 se a posição for liberada; 0 caso contrário.
//Pré-condição: posicao deve identificar um registro existente e ocupado.
//Pós-condição: a posição passa a ser a cabeça da lista de livres.
int liberarRegistroDados(int posicao) {
    FILE *arq;
    CabecalhoDados cab;
    RegistroDados reg;
    int ok;

    arq = fopen(ARQUIVO_DADOS, "rb+");
    if (arq == NULL)
        return 0;

    if (!lerCabDadosAberto(arq, &cab) || posicao < 0 || posicao >= cab.topo) {
        fclose(arq);
        return 0;
    }

    if (fseek(arq, deslocamentoDados(posicao), SEEK_SET) != 0 ||
        fread(&reg, sizeof(reg), 1, arq) != 1) {
        fclose(arq);
        return 0;
    }

    reg.proxLivre = cab.livres;
    if (fseek(arq, deslocamentoDados(posicao), SEEK_SET) != 0 ||
        fwrite(&reg, sizeof(reg), 1, arq) != 1) {
        fclose(arq);
        return 0;
    }

    cab.livres = posicao;
    ok = gravarCabDadosAberto(arq, &cab);
    if (fclose(arq) != 0)
        ok = 0;
    return ok;
}

//Lê um nó da árvore B pela posição lógica.
//Entrada: posição lógica do nó e ponteiro para receber o nó.
//Retorno: 1 se a leitura for realizada; 0 caso contrário.
//Pré-condição: posicao deve ser não negativa e no deve ser válido.
//Pós-condição: no contém o nó lido do arquivo de índices.
int lerNoIndice(int posicao, NoB *no) {
    FILE *arq;
    CabecalhoIndices cab;
    int ok;

    if (posicao < 0 || no == NULL)
        return 0;

    arq = fopen(ARQUIVO_INDICES, "rb");
    if (arq == NULL)
        return 0;

    if (!lerCabIndicesAberto(arq, &cab) || posicao >= cab.topo) {
        fclose(arq);
        return 0;
    }

    ok = fseek(arq, deslocamentoIndice(posicao), SEEK_SET) == 0 &&
         fread(no, sizeof(*no), 1, arq) == 1;
    fclose(arq);
    return ok;
}


//Grava um nó da árvore B em uma posição lógica.
//Entrada: posição lógica e ponteiro constante para o nó.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: posicao deve ser não negativa e no deve ser válido.
//Pós-condição: o nó é gravado na posição informada.
int gravarNoIndice(int posicao, const NoB *no) {
    FILE *arq;
    CabecalhoIndices cab;
    int ok;

    if (posicao < 0 || no == NULL)
        return 0;

    arq = fopen(ARQUIVO_INDICES, "rb+");
    if (arq == NULL)
        return 0;

    if (!lerCabIndicesAberto(arq, &cab) || posicao > cab.topo) {
        fclose(arq);
        return 0;
    }

    ok = fseek(arq, deslocamentoIndice(posicao), SEEK_SET) == 0 &&
         fwrite(no, sizeof(*no), 1, arq) == 1;

    // Atualiza o topo e verifica se o cabecalho foi gravado corretamente.
    if (ok && posicao >= cab.topo - 1) {
        cab.topo = posicao + 1;
        ok = gravarCabIndicesAberto(arq, &cab);
    }

    if (fclose(arq) != 0)
        ok = 0;
    return ok;
}


//Obter uma posicao para um novo no da Arvore B.
//Entrada: Nao possui parametros.
//Retorno: Posicao disponivel ou -1 em caso de erro.
//Pre-condicao: O arquivo de indices deve estar inicializado.
//Pos-condicao: A posicao e retirada da lista de livres ou o topo e avancado.
int alocarNoIndice(void) {
    CabecalhoIndices cab;
    NoB no;
    FILE *arq;
    int pos;
    int leituraOk;

    if (!lerCabecalhoIndices(&cab))
        return -1;

    if (cab.livres != -1) {
        pos = cab.livres;

        if (pos < 0 || pos >= cab.topo)
            return -1;

        arq = fopen(ARQUIVO_INDICES, "rb");
        if (arq == NULL)
            return -1;

        leituraOk = fseek(arq, deslocamentoIndice(pos), SEEK_SET) == 0 &&
                    fread(&no, sizeof(no), 1, arq) == 1;

        if (fclose(arq) != 0)
            leituraOk = 0;

        if (!leituraOk)
            return -1;

        cab.livres = no.proxLivre;

        if (!gravarCabecalhoIndices(&cab))
            return -1;

        return pos;
    }

    pos = cab.topo;
    cab.topo++;

    if (!gravarCabecalhoIndices(&cab))
        return -1;

    return pos;
}

//Devolve uma posição de nó à lista de nós livres.
//Entrada: posição lógica a liberar.
//Retorno: 1 se a posição for liberada; 0 caso contrário.
//Pré-condição: posicao deve identificar um nó existente e ocupado.
//Pós-condição: a posição passa a ser a cabeça da lista de nós livres.
int liberarNoIndice(int posicao) {
    FILE *arq;
    CabecalhoIndices cab;
    NoB no;
    int ok;

    arq = fopen(ARQUIVO_INDICES, "rb+");
    if (arq == NULL)
        return 0;

    if (!lerCabIndicesAberto(arq, &cab) || posicao < 0 || posicao >= cab.topo) {
        fclose(arq);
        return 0;
    }

    if (fseek(arq, deslocamentoIndice(posicao), SEEK_SET) != 0 ||
        fread(&no, sizeof(no), 1, arq) != 1) {
        fclose(arq);
        return 0;
    }

    no.proxLivre = cab.livres;
    if (fseek(arq, deslocamentoIndice(posicao), SEEK_SET) != 0 ||
        fwrite(&no, sizeof(no), 1, arq) != 1) {
        fclose(arq);
        return 0;
    }

    cab.livres = posicao;
    ok = gravarCabIndicesAberto(arq, &cab);
    if (fclose(arq) != 0)
        ok = 0;
    return ok;
}


//Grava um novo produto no arquivo de dados.
//Entrada: produto e ponteiro para receber a posição lógica.
//Retorno: 1 se a gravação for realizada; 0 caso contrário.
//Pré-condição: produto e posicao devem ser válidos e os arquivos inicializados.
//Pós-condição: o produto é gravado e sua posição lógica é retornada.
int gravarNovoProduto(const Produto *produto, int *posicao)
{
    RegistroDados registro;
    int novaPosicao;

    if (produto == NULL || posicao == NULL)
        return 0;

    if (!validarProduto(produto))
        return 0;

    novaPosicao = alocarRegistroDados();

    if (novaPosicao < 0)
        return 0;

    registro.produto = *produto;
    registro.proxLivre = -1;

    if (!gravarRegistroDados(novaPosicao, &registro)) {
        liberarRegistroDados(novaPosicao);
        return 0;
    }

    *posicao = novaPosicao;
    return 1;
}

//Imprime a lista de nos livres do arquivo de indices.
//Entrada: Nao possui parametros.
//Retorno: Nao possui retorno.
//Pre-condicao: O arquivo de indices deve estar inicializado.
//Pos-condicao: As posicoes dos nos livres sao exibidas na tela.
void imprimirListaLivresIndices(void){
    CabecalhoIndices cab;
    NoB no;
    int posicao;
    int quantidade = 0;

    if (!lerCabecalhoIndices(&cab)) {
        printf("Erro ao ler o arquivo de indices.\n");
        return;
    }

    posicao = cab.livres;
    printf("\nLista de nos livres do arquivo de indices:\n");

    if (posicao == -1) {
        printf("Lista vazia.\n");
        return;
    }

    while (posicao != -1) {
        if (posicao < 0 || posicao >= cab.topo ||
            quantidade >= cab.topo) {
            printf("\nErro: lista de livres invalida.\n");
            return;
        }

        if (!lerNoIndice(posicao, &no)) {
            printf("\nErro ao ler um no livre.\n");
            return;
        }

        printf("Posicao [%d]", posicao);

        posicao = no.proxLivre;
        quantidade++;

        if (posicao != -1)
            printf(" -> ");
    }
}

//Imprime a lista de registros livres do arquivo de dados.
//Entrada: Nao possui parametros.
//Retorno: Nao possui retorno.
//Pre-condicao: O arquivo de dados deve estar inicializado.
//Pos-condicao: As posicoes dos registros livres sao exibidas na tela.
void imprimirListaLivresDados(void){
    CabecalhoDados cab;
    RegistroDados registro;
    int posicao;
    int quantidade = 0;

    if (!lerCabecalhoDados(&cab)) {
        printf("Erro ao ler o arquivo de dados.\n");
        return;
    }

    posicao = cab.livres;
    printf("\nLista de registros livres do arquivo de dados:\n");

    if (posicao == -1) {
        printf("Lista vazia.\n");
        return;
    }

    while (posicao != -1) {
        if (posicao < 0 || posicao >= cab.topo ||
            quantidade >= cab.topo) {
            printf("\nErro: lista de livres invalida.\n");
            return;
        }

        if (!lerRegistroDados(posicao, &registro)) {
            printf("\nErro ao ler um registro livre.\n");
            return;
        }

        printf("Posicao [%d]", posicao);

        posicao = registro.proxLivre;
        quantidade++;

        if (posicao != -1)
            printf(" -> ");
    }
}