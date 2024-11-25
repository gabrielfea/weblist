#include "weblist_pub.h"
#include "DDLL_pub.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>  // Necessário para gerar números aleatórios

// Função para imprimir os resultados dos testes
void print_test_result(const char *func_name, int result) {
    printf("%s = %s\n", func_name, (result == SUCCESS) ? "SUCCESS" : "FAIL");
}

#define PRINT_TEST_RESULT(func_name, result) \
    print_test_result(func_name, result)

// Função de comparação para strings
int cmpString(void *a, void *b) {
    char *strA = *(char **)a;
    char *strB = *(char **)b;
    return strcmp(strA, strB);
}

// Função para imprimir strings
void printString(void *element) {
    if (element == NULL) {
        printf("(nulo)");
        return;
    }
    printf("%s", *(char **)element);
}

// Função de comparação para inteiros
int cmpInt(void *a, void *b) {
    int intA = *(int *)a;
    int intB = *(int *)b;
    return (intA > intB) - (intA < intB);
}

// Função para imprimir inteiros
void printInt(void *element) {
    if (element == NULL) {
        printf("(nulo)");
        return;
    }
    printf("%d", *(int *)element);
}

// Função para gerar uma string aleatória de tamanho especificado
char *generateRandomString(size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t charsetSize = sizeof(charset) - 1;

    if (length == 0) return NULL;

    char *str = (char *)malloc(length);
    if (str == NULL) {
        printf("Erro ao alocar memória para a string.\n");
        exit(1);
    }

    for (size_t i = 0; i < length - 1; i++) {
        int key = rand() % charsetSize;
        str[i] = charset[key];
    }
    str[length - 1] = '\0'; // Assegura terminação nula

    return str;
    
}

int main() {
    pweblist pWL = NULL;
    int result;
    int retorno;
    pDDLL novaLista = NULL;
    pDDLL listaChaves = NULL;

    pDDLL copiedList = NULL;
    pDDLL removedList = NULL;

    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // Teste de criação da WebList
    result = cWL(&pWL, 2, 20);
    PRINT_TEST_RESULT("cWL", result);
    if (result == FAIL) {
        return 1;  // Não faz sentido continuar se a criação falhou
    }

    // Inserir 1000 inteiros aleatórios na WebList
    for (int i = 0; i < 1000; i++) {
        int randomValue = rand() % 10000;  // Gera um número aleatório entre 0 e 9999
        result = iDado(pWL, &randomValue, cmpInt);
        if (result != SUCCESS) {
            printf("Falha ao inserir o valor %d na WebList.\n", randomValue);
        }
    }
    printf("Inserção de 1000 inteiros aleatórios concluída.\n");

    // Inserir 1000 strings aleatórias na WebList
        for (int i = 0; i < 1000; i++) {
            char *randomString = generateRandomString(11);  // Gera uma string aleatoria
            result = iDado(pWL, &randomString, cmpString);
            if (result != SUCCESS) {
                printf("Falha ao inserir a string %s na WebList.\n", randomString);
            } free(randomString);
        }
        printf("Inserção de 1000 strings aleatórias concluída.\n");
    
    // Teste de número total de elementos
    result = nroEleWL(pWL, &retorno);
    PRINT_TEST_RESULT("nroEleWL", result);
    if (result == SUCCESS) {
        printf("Número total de elementos na WebList após inserção: %d\n", retorno);
    }

    // Verificar se a WebList está balanceada antes do balanceamento
    result = WLbalanceada(pWL);
    PRINT_TEST_RESULT("WLbalanceada (antes do balanceamento)", result);

    // Teste de balanceamento da WebList
    result = balancearWL(pWL);
    PRINT_TEST_RESULT("balancearWL", result);

    // Verificar se a WebList está balanceada após o balanceamento
    result = WLbalanceada(pWL);
    PRINT_TEST_RESULT("WLbalanceada (após o balanceamento)", result);

    // Teste de número de elementos em cada folha
    result = lstChaves(pWL, &listaChaves);
    if (result == SUCCESS && listaChaves != NULL) {
        int numFolhas = countElements(listaChaves);
        int *elementosPorFolha = (int *)malloc(numFolhas * sizeof(int));
        if (elementosPorFolha == NULL) {
            printf("Erro ao alocar memória para elementosPorFolha.\n");
            dDDLL(&listaChaves);
            return 1;
        }

        int totalElementos = 0;
        for (int i = 1; i <= numFolhas; i++) {
            int chave;
            sPosition(listaChaves, i, &chave);
            int numElementosFolha = 0;
            nroEleNoFolha(pWL, chave, &numElementosFolha);
            elementosPorFolha[i - 1] = numElementosFolha;
            totalElementos += numElementosFolha;
        }

        // Calcular a média de elementos por folha
        double mediaElementos = (double)totalElementos / numFolhas;

        // Verificar a diferença máxima entre as folhas
        int maxElementos = elementosPorFolha[0];
        int minElementos = elementosPorFolha[0];
        for (int i = 0; i < numFolhas; i++) {
            if (elementosPorFolha[i] > maxElementos) {
                maxElementos = elementosPorFolha[i];
            }
            if (elementosPorFolha[i] < minElementos) {
                minElementos = elementosPorFolha[i];
            }
        }

        printf("Número total de folhas: %d\n", numFolhas);
        printf("Média de elementos por folha: %.2f\n", mediaElementos);
        printf("Máximo de elementos em uma folha: %d\n", maxElementos);
        printf("Mínimo de elementos em uma folha: %d\n", minElementos);

        // Limpar alocações temporárias
        free(elementosPorFolha);
        dDDLL(&listaChaves);
    } else {
        printf("Falha ao obter as chaves das folhas.\n");
    }

    // **Inserção de uma nova lista com 100 elementos em um nó folha**

    // Criar uma nova lista com 100 inteiros
    result = cDDLL(&novaLista, sizeof(int));
    if (result != SUCCESS) {
        printf("Falha ao criar nova lista.\n");
        dWL(&pWL);
        return 1;
    }
    for (int i = 0; i < 100; i++) {
        int value = rand() % 10000;  // Gera um número aleatório
        result = iEnd(novaLista, &value);
        if (result != SUCCESS) {
            printf("Falha ao inserir valor na nova lista.\n");
            dDDLL(&novaLista);
            dWL(&pWL);
            return 1;
        }
    }
    printf("Nova lista com 100 elementos criada.\n");

    // Obter uma chave de um nó folha para substituir a lista
    result = lstChaves(pWL, &listaChaves);
    if (result != SUCCESS || listaChaves == NULL) {
        printf("Falha ao obter as chaves das folhas.\n");
        dDDLL(&novaLista);
        dWL(&pWL);
        return 1;
    }
    // Vamos usar a primeira chave
    int chave;
    sPosition(listaChaves, 1, &chave);
    dDDLL(&listaChaves);  // Não precisamos mais da lista de chaves

    // Substituir a lista no nó folha com a nova lista
    result = sbLista(pWL, chave, novaLista);
    PRINT_TEST_RESULT("sbLista", result);

    // Verificar se a WebList está balanceada após inserir a nova lista
    result = WLbalanceada(pWL);
    PRINT_TEST_RESULT("WLbalanceada (após inserir nova lista)", result);
    if (result == FAIL) {
        printf("A WebList não está balanceada após inserir a nova lista.\n");
    }

    // Balancear a WebList novamente
    result = balancearWL(pWL);
    PRINT_TEST_RESULT("balancearWL (após inserir nova lista)", result);

    // Verificar se a WebList está balanceada após balanceamento
    result = WLbalanceada(pWL);
    PRINT_TEST_RESULT("WLbalanceada (após balanceamento pós-inserção)", result);

    // Atualizar o número total de elementos e informações das folhas
    result = nroEleWL(pWL, &retorno);
    PRINT_TEST_RESULT("nroEleWL (após inserção e balanceamento)", result);
    if (result == SUCCESS) {
        printf("Número total de elementos na WebList após inserção e balanceamento: %d\n", retorno);
    }

    // Atualizar informações de elementos por folha
    result = lstChaves(pWL, &listaChaves);
    if (result == SUCCESS && listaChaves != NULL) {
        int numFolhas = countElements(listaChaves);
        int *elementosPorFolha = (int *)malloc(numFolhas * sizeof(int));
        if (elementosPorFolha == NULL) {
            printf("Erro ao alocar memória para elementosPorFolha.\n");
            dDDLL(&listaChaves);
            dWL(&pWL);
            return 1;
        }

        int totalElementos = 0;
        for (int i = 1; i <= numFolhas; i++) {
            int chaveFolha;
            sPosition(listaChaves, i, &chaveFolha);
            int numElementosFolha = 0;
            nroEleNoFolha(pWL, chaveFolha, &numElementosFolha);
            elementosPorFolha[i - 1] = numElementosFolha;
            totalElementos += numElementosFolha;
        }

        // Calcular a média de elementos por folha
        double mediaElementos = (double)totalElementos / numFolhas;

        // Verificar a diferença máxima entre as folhas
        int maxElementos = elementosPorFolha[0];
        int minElementos = elementosPorFolha[0];
        for (int i = 0; i < numFolhas; i++) {
            if (elementosPorFolha[i] > maxElementos) {
                maxElementos = elementosPorFolha[i];
            }
            if (elementosPorFolha[i] < minElementos) {
                minElementos = elementosPorFolha[i];
            }
        }

        printf("Número total de folhas após inserção: %d\n", numFolhas);
        printf("Média de elementos por folha após inserção: %.2f\n", mediaElementos);
        printf("Máximo de elementos em uma folha após inserção: %d\n", maxElementos);
        printf("Mínimo de elementos em uma folha após inserção: %d\n", minElementos);

        // Limpar alocações temporárias
        free(elementosPorFolha);
        dDDLL(&listaChaves);
    } else {
        printf("Falha ao obter as chaves das folhas após inserção.\n");
    }

    // Teste de destruição da WebList
    result = dWL(&pWL);
    PRINT_TEST_RESULT("dWL", result);

    // Limpeza final, caso necessário
    if (removedList != NULL) {
        dDDLL(&removedList);
    }
    if (copiedList != NULL) {
        dDDLL(&copiedList);
    }
    // Não precisamos liberar novaLista, pois ela foi inserida na WebList

    int data = 42;
    int data2 = 51;
    int data3 = 45189;
    int dataBusca = 42;
    int dataNaoExistente = 99;


    // Teste de criação da WebList
    result = cWL(&pWL, 2, sizeof(int));
    PRINT_TEST_RESULT("cWL", result);
    if (result == FAIL) {
        return 1;  // Não faz sentido continuar se a criação falhou
    }

    // Teste de inserção de dados na WebList
    result = iDado(pWL, &data, cmpInt);
    PRINT_TEST_RESULT("iDado (data)", result);

    result = iDado(pWL, &data2, cmpInt);
    PRINT_TEST_RESULT("iDado (data2)", result);

    result = iDado(pWL, &data3, cmpInt);
    PRINT_TEST_RESULT("iDado (data3)", result);

    // Teste de busca de um dado existente
    result = bDado(pWL, &dataBusca, cmpInt);
    PRINT_TEST_RESULT("bDado (existente)", result);

    // Teste de busca de um dado inexistente
    result = bDado(pWL, &dataNaoExistente, cmpInt);
    PRINT_TEST_RESULT("bDado (inexistente)", result);

    // Teste de número de elementos em uma folha
    result = nroEleNoFolha(pWL, 1, &retorno);
    PRINT_TEST_RESULT("nroEleNoFolha", result);
    if (result == SUCCESS) {
        printf("Número de elementos na folha 1: %d\n", retorno);
    }

    // Teste de número total de nós folha
    result = nroNoFolha(pWL, &retorno);
    PRINT_TEST_RESULT("nroNoFolha", result);
    if (result == SUCCESS) {
        printf("Número total de folhas: %d\n", retorno);
    }

    // Teste de número total de elementos
    result = nroEleWL(pWL, &retorno);
    PRINT_TEST_RESULT("nroEleWL", result);
    if (result == SUCCESS) {
        printf("Número total de elementos na WebList: %d\n", retorno);
    }

    // Teste de impressão dos elementos
    printf("Elementos na WebList:\n");
    result = pLista(pWL, printInt);
    PRINT_TEST_RESULT("pLista", result);

    // Teste se a WebList está balanceada
    result = WLbalanceada(pWL);
    PRINT_TEST_RESULT("WLbalanceada", result);

    // Teste de remoção de um dado existente
    result = rDado(pWL, &data, cmpInt);
    PRINT_TEST_RESULT("rDado (existente)", result);

    // Teste de remoção de um dado inexistente
    result = rDado(pWL, &dataNaoExistente, cmpInt);
    PRINT_TEST_RESULT("rDado (inexistente)", result);

    // Teste de impressão dos elementos após remoção
    printf("Elementos na WebList após remoção:\n");
    result = pLista(pWL, printInt);
    PRINT_TEST_RESULT("pLista", result);

    // Teste de cpLista - copiar a lista de um nó folha
    result = cpLista(pWL, 1, &copiedList);
    PRINT_TEST_RESULT("cpLista", result);
    if (result == SUCCESS) {
        printf("Lista copiada da folha 1: ");
        printDDLL(copiedList, printInt);
        dDDLL(&copiedList); // Liberar a lista copiada após o uso
    }

    // Criar nova lista 'novaLista' e inserir dados
    result = cDDLL(&novaLista, sizeof(int));
    if (result == SUCCESS) {
        int newData1 = 100;
        int newData2 = 200;
        iEnd(novaLista, &newData1);
        iEnd(novaLista, &newData2);
    } else {
        printf("Falha ao criar novaLista.\n");
    }

    // Teste de substituição de uma lista em um nó da WebList
    result = sbLista(pWL, 1, novaLista);
    PRINT_TEST_RESULT("sbLista", result);
    if (result == SUCCESS) {
        printf("WebList após sbLista:\n");
        pLista(pWL, printInt);
        // 'novaLista' is now managed by the WebList; set to NULL to avoid double free
        novaLista = NULL;
    }

    // Teste de remoção de uma lista em um nó da WebList
    result = rmLista(pWL, 1, &removedList);
    PRINT_TEST_RESULT("rmLista", result);
    if (result == SUCCESS && removedList != NULL) {
        printf("Lista removida da folha 1: ");
        printDDLL(removedList, printInt);
        dDDLL(&removedList);  // Liberar a lista removida
    }

    // Teste de criação de uma nova lista em um nó da WebList
    result = nvLista(pWL, 1);  // Tenta criar uma nova lista com chave 1
    PRINT_TEST_RESULT("nvLista", result);
    if (result == SUCCESS) {
        printf("Nova lista criada na chave 1.\n");
        // Opcionalmente, imprimir a WebList para verificar
        pLista(pWL, printInt);
    } else {
        printf("Falha ao criar nova lista na chave 1.\n");
    }

    // Teste de lstChaves - listar todas as chaves das folhas da WebList
    result = lstChaves(pWL, &listaChaves);
    PRINT_TEST_RESULT("lstChaves", result);
    if (result == SUCCESS && listaChaves != NULL) {
        printf("Chaves encontradas: ");
        printDDLL(listaChaves, printInt);
        dDDLL(&listaChaves);  // Limpar a lista de chaves após o uso
    }

    // Teste de balanceamento da WebList
    result = balancearWL(pWL);
    PRINT_TEST_RESULT("balancearWL", result);

    // Imprimir a WebList após balanceamento
    printf("WebList após balancearWL:\n");
    pLista(pWL, printInt);

    // Teste de destruição da WebList
    result = dWL(&pWL);
    PRINT_TEST_RESULT("dWL", result);

    // Limpeza final, caso necessário
    if (novaLista != NULL) {
        dDDLL(&novaLista);
    }
    if (removedList != NULL) {
        dDDLL(&removedList);
    }
    if (copiedList != NULL) {
        dDDLL(&copiedList);
    }
    if (listaChaves != NULL) {
        dDDLL(&listaChaves);
    }

    return 0;
}
