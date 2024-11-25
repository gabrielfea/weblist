#ifndef WEBLIST_PRI_H
#define WEBLIST_PRI_H

#include "weblist_pub.h"
#include "DDLL_pub.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SUCCESS 0
#define FAIL -1

// Definindo a estrutura do nó WebList (NoWebList)
typedef struct NoWebList {
    int key;  // Chave formada de 1 até 8^(nivel+1).
    pDDLL lista;  // Ponteiro para a lista DDLL associada.
} NoWebList;

// Definindo a estrutura da WebList
typedef struct weblist {
    size_t sizedata; //tamanho do dado na DDLL;
    int root; // 1 se root e 0 se não;
    int g_key_counter; //Variável que ajuda na criação das Keys folhas.
    int ult_mov;  // Armazenar a chave do último nó folha movimentado em inserção e diminui se algo for removido. Mantém balance.    
    int nivel;  // Nível da WebList, se for nivel -1 é uma folha.
    union {
        struct weblist *filhos[8]; // Array de ponteiros para WebLists (caso não seja o último nível).
        NoWebList *noFolha;        // Ponteiro para NoWebList (caso seja o último nível: -1).
    } conteudo;
} weblist;

#endif // WEBLIST_PRI_H 