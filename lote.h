#ifndef LOTE_H
#define LOTE_H

//Processa as operações de um arquivo de lote.
//Entrada: nome do arquivo de texto que contém as operações.
//Retorno: 1 se o processamento for realizado; 0 caso ocorra erro.
//Pré-condição: nomeArquivo deve indicar um arquivo de lote válido.
//Pós-condição: as operações válidas do arquivo são processadas no sistema.
int operacoes(const char *nomeArquivo);

#endif
