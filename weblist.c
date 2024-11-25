#include "weblist_pri.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Função auxiliar recursiva para criar a WebList
int cWLRecursivo(pweblist current, int nivel, int *key_counter, int sizedata) {
    if (nivel > -1) {
        // Inicializa os filhos recursivamente
        for (int i = 0; i < 8; i++) {
            current->conteudo.filhos[i] = (pweblist)malloc(sizeof(struct weblist));
            if (current->conteudo.filhos[i] == NULL) {
                return FAIL;  // Falha ao alocar memória
            }
            current->conteudo.filhos[i]->nivel = nivel - 1;
            current->conteudo.filhos[i]->g_key_counter = 0;
            current->conteudo.filhos[i]->ult_mov = 0;
            current->conteudo.filhos[i]->root = 0;
            current->conteudo.filhos[i]->sizedata = sizedata;
            // Chamada recursiva para os filhos
            if (cWLRecursivo(current->conteudo.filhos[i], nivel - 1, key_counter, sizedata) == FAIL) {
                return FAIL;
            }
        }
    } else {
        // Aloca os nós folha (nível -1)
        current->conteudo.noFolha = (NoWebList *)malloc(sizeof(NoWebList));
        if (current->conteudo.noFolha == NULL) {
            return FAIL;  // Falha ao alocar memória
        }
        //Coloca a chave, que vai sendo gerada pela raiz e lá se armazena a quantidade de nós folhas criados.
        current->conteudo.noFolha->key = (*key_counter)++;
        if (cDDLL(&current->conteudo.noFolha->lista, sizedata) != SUCCESS) {
            free(current->conteudo.noFolha);
            return FAIL;  // Falha ao criar DDLL
        }
    }
    return SUCCESS;
}

// Função para criar a WebList
int cWL(ppweblist ppWL, int nivel, int sizedata) {
    if (ppWL == NULL || sizedata <= 0 || nivel < 0) {
        return FAIL;
    }

    // Aloca a raiz da WebList
    *ppWL = (pweblist)malloc(sizeof(struct weblist));
    if (*ppWL == NULL) {
        return FAIL;
    }

    (*ppWL)->nivel = nivel;
    (*ppWL)->g_key_counter = 1;
    (*ppWL)->ult_mov = 0;
    (*ppWL)->root = 1;
    (*ppWL)->sizedata = sizedata;

    // Chama a função recursiva para criar os filhos da raiz
    if (cWLRecursivo(*ppWL, nivel, &((*ppWL)->g_key_counter), sizedata) == FAIL) {
        free(*ppWL);
        return FAIL;
    }

    return SUCCESS;
}

// Função auxiliar recursiva para destruir a WebList
void dWLRecursivo(pweblist current) {
    if (current == NULL) {
        return;
    }

    if (current->nivel > -1) {
        // Destrói os filhos recursivamente
        for (int i = 0; i < 8; i++) {
            if (current->conteudo.filhos[i] != NULL) {
                dWLRecursivo(current->conteudo.filhos[i]);
                free(current->conteudo.filhos[i]);
            }
        }
    } else {
        // Destrói o nó folha
        if (current->conteudo.noFolha != NULL) {
            dDDLL(&current->conteudo.noFolha->lista);
            free(current->conteudo.noFolha);
        }
    }
}

// Função para destruir a WebList
int dWL(ppweblist ppWL) {
    if (ppWL == NULL || *ppWL == NULL) {
        return FAIL;
    }

    // Chama a função recursiva para destruir a árvore
    dWLRecursivo(*ppWL);

    // Libera a raiz
    free(*ppWL);
    *ppWL = NULL;

    return SUCCESS;
}

// Função para encontrar a folha que contém o elemento e sua posição
pweblist encontrarFolhaComElemento(pweblist raiz, void *element, int (*cmp)(void *, void *), int *posEncontrada) {
    if (raiz->nivel == -1) {
        // Estamos em um nó folha, verificar a lista
        pDDLL lista = raiz->conteudo.noFolha->lista;
        void *atual = NULL;
        int pos = 1;
        while (sPosition(lista, pos, &atual) == SUCCESS) {
            if (cmp(&atual, element) == 0) {
                *posEncontrada = pos; // Armazena a posição do elemento encontrado
                return raiz; // Retorna a folha que contém o elemento
            }
            pos++;
        }
        return NULL; // Elemento não encontrado nesta folha
    }

    // Percorrer os filhos para encontrar a folha que possa conter o elemento
    for (int i = 0; i < 8; i++) {
        if (raiz->conteudo.filhos[i] != NULL) {
            pweblist resultado = encontrarFolhaComElemento(raiz->conteudo.filhos[i], element, cmp, posEncontrada);
            if (resultado != NULL) {
                return resultado;
            }
        }
    }
    return NULL; // Elemento não encontrado
}

// Função para encontrar a folha com a chave especificada
pweblist encontrarFolhaComChave(pweblist raiz, int key) {
    if (raiz->nivel == -1) {
        if (raiz->conteudo.noFolha->key == key) {
            return raiz;
        } else {
            return NULL;
        }
    }

    // Percorrer os filhos para encontrar a folha com a chave
    for (int i = 0; i < 8; i++) {
        if (raiz->conteudo.filhos[i] != NULL) {
            pweblist resultado = encontrarFolhaComChave(raiz->conteudo.filhos[i], key);
            if (resultado != NULL) {
                return resultado;
            }
        }
    }
    return NULL; // Folha com a chave não encontrada
}

// Função para inserir um dado na weblist
int iDado(pweblist pWL, void *element, int (*cmp)(void *, void *)) {
    if (pWL == NULL || element == NULL) {
        return FAIL;
    }
    pweblist pWL_original = pWL;
    
    // Obtém o número total de nós folha
    int totalFolhas;
    if (nroNoFolha(pWL, &totalFolhas) == FAIL) {
        return FAIL;
    }

    // Calcula a próxima chave (deve ser entre 1 e totalFolhas)
    int target_key = (pWL->ult_mov % totalFolhas) + 1;

    // Encontra o nó folha com a chave target_key
    pweblist folha = encontrarFolhaComChave(pWL, target_key);
    if (folha == NULL) {
        // Se não encontrou o nó folha com a chave target_key, falha
        return FAIL;
    }

    // Insere o elemento na lista DDLL associada ao nó folha
    if (iEnd(folha->conteudo.noFolha->lista, element) != SUCCESS) {
        return FAIL;
    }

    // Atualiza a variável para armazenar a chave do último nó folha movimentado
    pWL_original->ult_mov++;
    
    return SUCCESS;
}

// Função para remover um dado na weblist
int rDado(pweblist pWL, void *element, int (*cmp)(void *, void *)) {
    if (pWL == NULL || element == NULL) {
        return FAIL;
    }
    pweblist pWL_original = pWL;
    
    // Localizar a folha que contém o elemento e a posição
    int posEncontrada = -1;
    pweblist folhaComElemento = encontrarFolhaComElemento(pWL, element, cmp, &posEncontrada);
    if (folhaComElemento == NULL) {
        return FAIL; // Elemento não encontrado
    }

    // Remove o elemento da lista DDLL da folha encontrada usando a posição armazenada
    if (rPosition(folhaComElemento->conteudo.noFolha->lista, posEncontrada, element) != SUCCESS) {
        return FAIL;
    }

    // Obtém o número total de nós folha
    int totalFolhas;
    if (nroNoFolha(pWL, &totalFolhas) == FAIL) {
        return FAIL;
    }

    // Calcula a chave do último nó movimentado
    int target_key = (pWL->ult_mov % totalFolhas);

    // Verifica se o dado foi removido do último nó movimentado
    int keyRemovedNode = folhaComElemento->conteudo.noFolha->key;
    if (keyRemovedNode != target_key) {
        // O dado não foi removido do último nó movimentado
        // Encontrar o último nó movimentado
        pweblist lastMovedNode = encontrarFolhaComChave(pWL, target_key);
        if (lastMovedNode == NULL) {
            return FAIL;
        }

        // Remover um dado do último nó movimentado
        void *dataToMove = NULL;
        int posToRemove = 1; // Por exemplo, removemos o primeiro elemento
        if (sPosition(lastMovedNode->conteudo.noFolha->lista, posToRemove, &dataToMove) == SUCCESS) {
            // Remove o dado do último nó movimentado
            if (rPosition(lastMovedNode->conteudo.noFolha->lista, posToRemove, &dataToMove) != SUCCESS) {
                return FAIL;
            }

            // Inserir o dado no nó onde o dado foi removido
            if (iEnd(folhaComElemento->conteudo.noFolha->lista, &dataToMove) != SUCCESS) {
                return FAIL;
            }
        } else {
            // Não há dados para mover
            return FAIL;
        }
    }

    return SUCCESS;
}

// Função para imprimir todos os elementos da WebList presentes em todas as suas DDLLs, usando função de impressão
int pLista(pweblist pWL, void (*printElement)(void *)) {
    if (pWL == NULL) {
        printf("WebList está vazia.\n");
        return FAIL;
    }
    
    if (pWL->nivel > -1) {
            for (int i = 0; i < 8; i++) {
                if (pWL->conteudo.filhos[i] != NULL) {
                    pLista(pWL->conteudo.filhos[i], printElement);
                }
            }
    } else {
        if (pWL->conteudo.noFolha != NULL) {
            printf("Chave da folha: %d || Elementos: ", pWL->conteudo.noFolha->key);
            printDDLL(pWL->conteudo.noFolha->lista, printElement);
            //printf(" || Qtde de Elementos: { %d }\n",countElements(pWL->conteudo.noFolha->lista));
        }
    }
    return SUCCESS;
}

// Função para imprimir todos os elementos de uma lista DDLL
void printDDLL(pDDLL lista, void (*printElement)(void *)) {
    if (lista == NULL) {
        printf("Lista DDLL está vazia.\n");
        return;
    }

    void *atual = NULL;
    int pos = 1;
    int primeiroElemento = 1;

    printf("{ ");
    while (sPosition(lista, pos, &atual) != FAIL) {
        if (!primeiroElemento) {
            printf(", ");
        }
        printElement(&atual);
        primeiroElemento = 0;
        pos++;
    }
    
    printf(" }\n");
    
}


//Retorna o número de elementos em um nó folha específico.
int nroEleNoFolha(pweblist pWL, int chave, int *retorno) {
    if (pWL == NULL || retorno == NULL) {
        return FAIL;
    }

    // Utilizando uma pilha para realizar uma busca manual em profundidade
    pweblist stack[1024]; // Tamanho arbitrário da pilha, ajuste conforme necessário
    int top = -1;

    // Inicializar a pilha com o nó raiz
    stack[++top] = pWL;

    while (top >= 0) {
        pweblist current = stack[top--];

        if (current->nivel == -1) {
            // Se o nó atual for uma folha, verificar a chave
            if (current->conteudo.noFolha->key == chave) {
                *retorno = countElements(current->conteudo.noFolha->lista);
                return SUCCESS;
            }
        } else {
            // Caso contrário, adicionar os filhos à pilha para continuar a busca
            for (int i = 0; i < 8; i++) {
                if (current->conteudo.filhos[i] != NULL) {
                    stack[++top] = current->conteudo.filhos[i];
                }
            }
        }
    }

    return FAIL;
}

//Retorna o número total de elementos em toda a WebList.
int nroEleWL(pweblist pWL, int *retorno) {
    if (pWL == NULL || retorno == NULL) {
        return FAIL;
    }

    *retorno = 0;

    // Percorrer a WebList e somar os elementos de todas as folhas
    if (pWL->nivel > -1) {
        for (int i = 0; i < 8; i++) {
            if (pWL->conteudo.filhos[i] != NULL) {
                int subtotal = 0;
                nroEleWL(pWL->conteudo.filhos[i], &subtotal);
                *retorno += subtotal;
            }
        }
    } else {
        *retorno += countElements(pWL->conteudo.noFolha->lista);
    }

    return SUCCESS;
}

// Função auxiliar da WLbalanceada
int WLbalanceadaAux(pweblist pWL, int mediaElementos) {
    if (pWL == NULL) {
        return SUCCESS;
    }

    // Se é uma folha, verificar o número de elementos
    if (pWL->nivel == -1) {
        int elementosFolha = countElements(pWL->conteudo.noFolha->lista);
        if (abs(elementosFolha - mediaElementos) > 1) {
            return FAIL;
        }
        return SUCCESS;
    }

    // Se não é uma folha, verificar recursivamente os filhos
    for (int i = 0; i < 8; i++) {
        if (pWL->conteudo.filhos[i] != NULL) {
            if (WLbalanceadaAux(pWL->conteudo.filhos[i], mediaElementos) == FAIL) {
                return FAIL;
            }
        }
    }

    return SUCCESS;
}

//Verifica se a WebList está balanceada em termos de distribuição de elementos nos nós folha.
int WLbalanceada(pweblist pWL) {
    if (pWL == NULL) {
        return FAIL;
    }

    int totalElementos = 0;
    nroEleWL(pWL, &totalElementos);

    int folhas = 0;
    nroNoFolha(pWL, &folhas);

    if (folhas == 0) {
        return FAIL;
    }

    int mediaElementos = totalElementos / folhas;

    // Verificar se cada folha tem um número de elementos próximo da média
    return WLbalanceadaAux(pWL, mediaElementos);
}

// Função para retornar o número total de nós folha da estrutura WebList
int nroNoFolha(pweblist pWL, int *retorno) {
    if (pWL == NULL || retorno == NULL) {
        return FAIL;
    }

    *retorno = pWL->g_key_counter-1;
    
    return SUCCESS;
}

// Função para buscar um dado na estrutura WebList (versão recursiva)
int bDado(pweblist pWL, void *element, int (*cmp)(void *, void *)) {
    // Verificar argumentos inválidos
    if (pWL == NULL || element == NULL || cmp == NULL) {
        return FAIL;
    }

    // Se o nível é maior que -1, estamos em um nó intermediário
    if (pWL->nivel > -1) {
        // Percorrer todos os filhos do nó atual
        for (int i = 0; i < 8; i++) {
            if (pWL->conteudo.filhos[i] != NULL) {
                // Chamada recursiva para cada filho
                if (bDado(pWL->conteudo.filhos[i], element, cmp) == SUCCESS) {
                    return SUCCESS;
                }
            }
        }
    } else { // Caso contrário, estamos em um nó folha
        if (pWL->conteudo.noFolha != NULL) {
            void *atual = NULL;
            int pos = 1;

            // Percorrer a lista de elementos no nó folha
            while (sPosition(pWL->conteudo.noFolha->lista, pos, &atual) != FAIL) {
                // Comparar o elemento atual com o elemento buscado
                if (cmp(&atual, element) == 0) { // Considerando que cmp retorna 0 para igualdade
                    return SUCCESS;
                }
                pos++;
            }
        }
    }

    // Retornar falha se o elemento não foi encontrado
    return FAIL;
}

// Função auxiliar do listachaves.
int lstChavesRec(pweblist pWL, ppDDLL retorno) {
    if (pWL == NULL || retorno == NULL) {
        return FAIL;
    }

    // Verificar se o nó atual não é uma folha (nível maior que -1)
    if (pWL->nivel > -1) {
        // Percorrer todos os filhos do nó atual
        for (int i = 0; i < 8; i++) {
            if (pWL->conteudo.filhos[i] != NULL) {
                // Chamada recursiva para coletar as chaves dos filhos
                if (lstChavesRec(pWL->conteudo.filhos[i], retorno) != SUCCESS) {
                    return FAIL;
                }
            }
        }
    } else {
        // Caso seja uma folha, adicionar a chave na lista de retorno
        if (pWL->conteudo.noFolha != NULL) {
            int *chave = malloc(sizeof(int));
            if (chave == NULL) {
                return FAIL;
            }
            *chave = pWL->conteudo.noFolha->key;
            //printf("Adicionando chave: %d\n", *chave); // Depuração: Exibir a chave que está sendo adicionada
            if (iEnd(*retorno, (void *)chave) != SUCCESS) {
                free(chave);
                return FAIL;
            } free(chave);
        } else {
            return FAIL;  // Falha se noFolha for NULL em um nó folha
        }
    }
    return SUCCESS;
}

//Retorna uma lista com as chaves de todos os nós folha.
int lstChaves(pweblist pWL, ppDDLL retorno) {
    if (pWL == NULL || retorno == NULL) {
        return FAIL;
    }

    // Inicializar a lista de chaves apenas uma vez
    if (cDDLL(retorno, sizeof(int)) != SUCCESS) {
        return FAIL;
    }

    // Chamar a função recursiva auxiliar
    if (lstChavesRec(pWL, retorno) != SUCCESS) {
        dDDLL(retorno);  // Limpa a lista em caso de falha
        return FAIL;
    }

    return SUCCESS;
}

// Realiza o balanceamento manual da WebList, redistribuindo os elementos entre os nós folha
int balancearWL(pweblist pWL) {
    if (pWL == NULL) {
        return FAIL;
    }

    // Obter o número total de elementos
    int totalElementos = 0;
    if (nroEleWL(pWL, &totalElementos) != SUCCESS) {
        return FAIL;
    }

    // Obter o número de folhas
    int folhas = 0;
    if (nroNoFolha(pWL, &folhas) != SUCCESS || folhas == 0) {
        return FAIL;
    }

    // Calcular o número mínimo e máximo de elementos por folha
    int minElementsPerLeaf = totalElementos / folhas;
    int extraElements = totalElementos % folhas; // Número de folhas que terão um elemento extra
    int maxElementsPerLeaf = minElementsPerLeaf + 1;

    // Criar uma lista de chaves de todas as folhas
    pDDLL listaChaves = NULL; // Inicializar a lista
    if (lstChaves(pWL, &listaChaves) != SUCCESS) {
        return FAIL;
    }

    // Criar um array para armazenar o número alvo de elementos para cada folha
    int numLeaves = countElements(listaChaves);
    int *targetElements = (int *)malloc(sizeof(int) * numLeaves);

    if (targetElements == NULL) {
        dDDLL(&listaChaves);
        return FAIL;
    }

    // Atribuir o número alvo de elementos para cada folha
    for (int idx = 1; idx <= numLeaves; idx++) {
        if (idx <= extraElements) {
            targetElements[idx - 1] = maxElementsPerLeaf;
        } else {
            targetElements[idx - 1] = minElementsPerLeaf;
        }
    }

    // Variável para controlar se ocorreu alguma mudança
    int houveMudanca = 1;

    // Loop até que todas as folhas estejam balanceadas
    while (houveMudanca) {
        houveMudanca = 0;

        // Primeiro, identificar folhas com mais elementos que o alvo
        for (int i = 1; i <= numLeaves; i++) {
            int chaveAtual;
            if (sPosition(listaChaves, i, &chaveAtual) != SUCCESS) {
                free(targetElements);
                dDDLL(&listaChaves);
                return FAIL;
            }

            int elementosFolhaAtual = 0;
            if (nroEleNoFolha(pWL, chaveAtual, &elementosFolhaAtual) != SUCCESS) {
                free(targetElements);
                dDDLL(&listaChaves);
                return FAIL;
            }

            // Compare com o número alvo de elementos
            if (elementosFolhaAtual > targetElements[i - 1]) {
                // Encontrar uma folha com menos elementos que o seu alvo
                for (int j = 1; j <= numLeaves; j++) {
                    if (j == i) {
                        continue;
                    }

                    int chaveDestino;
                    if (sPosition(listaChaves, j, &chaveDestino) != SUCCESS) {
                        free(targetElements);
                        dDDLL(&listaChaves);
                        return FAIL;
                    }

                    int elementosFolhaDestino = 0;
                    if (nroEleNoFolha(pWL, chaveDestino, &elementosFolhaDestino) != SUCCESS) {
                        free(targetElements);
                        dDDLL(&listaChaves);
                        return FAIL;
                    }

                    if (elementosFolhaDestino < targetElements[j - 1]) {
                        // Mover um elemento da folha atual para a folha destino
                        pweblist folhaAtual = encontrarFolhaComChave(pWL, chaveAtual);
                        pweblist folhaDestino = encontrarFolhaComChave(pWL, chaveDestino);

                        if (folhaAtual == NULL || folhaDestino == NULL) {
                            free(targetElements);
                            dDDLL(&listaChaves);
                            return FAIL;
                        }

                        void *elemento = malloc(pWL->sizedata);
                        if (elemento == NULL) {
                            free(targetElements);
                            dDDLL(&listaChaves);
                            return FAIL;
                        }

                        // Remover elemento da folha atual
                        if (rPosition(folhaAtual->conteudo.noFolha->lista, 1, elemento) != SUCCESS) {
                            free(elemento);
                            free(targetElements);
                            dDDLL(&listaChaves);
                            return FAIL;
                        }

                        // Inserir elemento na folha destino
                        if (iEnd(folhaDestino->conteudo.noFolha->lista, elemento) != SUCCESS) {
                            free(elemento);
                            free(targetElements);
                            dDDLL(&listaChaves);
                            return FAIL;
                        }

                        free(elemento); // Libera memória alocada para o elemento

                        houveMudanca = 1; // Indica que houve mudança e precisamos verificar novamente

                        // Atualizar contagens
                        elementosFolhaAtual--;
                        elementosFolhaDestino++;

                        break; // Sai do loop interno para continuar o balanceamento
                    }
                }
            }
        }
    }

    // Liberar a memória alocada
    free(targetElements);
    dDDLL(&listaChaves);

    return SUCCESS;
}

//Copia a lista de um nó folha específico para uma nova lista.
int cpLista(pweblist pWL, int chave, ppDDLL retorno) {
    if (pWL == NULL || retorno == NULL) {
        return FAIL;
    }

    // Encontrar a folha com a chave especificada
    pweblist folha = encontrarFolhaComChave(pWL, chave);
    if (folha == NULL || folha->conteudo.noFolha == NULL || folha->conteudo.noFolha->lista == NULL) {
        return FAIL;
    }

    // Criar uma cópia da DDLL
    if (cDDLL(retorno, pWL->sizedata) != SUCCESS) {
        return FAIL;
    }

    pDDLL listaOrigem = folha->conteudo.noFolha->lista;
    void *elemento = malloc(pWL->sizedata);
    if (elemento == NULL) {
        dDDLL(retorno);
        return FAIL;
    }

    int pos = 1;
    while (sPosition(listaOrigem, pos++, elemento) == SUCCESS) {
        if (iEnd(*retorno, elemento) != SUCCESS) {
            dDDLL(retorno);  // Limpa a lista em caso de falha
            free(elemento);
            return FAIL;
        }
    }
    free(elemento);

    return SUCCESS;
}

//Substitui a lista de um nó folha por uma nova lista fornecida.
int sbLista(pweblist pWL, int chave, pDDLL novaLista) {
    if (pWL == NULL || novaLista == NULL) {
        return FAIL;
    }

    // Encontrar a folha com a chave especificada
    pweblist folha = encontrarFolhaComChave(pWL, chave);
    if (folha == NULL || folha->conteudo.noFolha == NULL) {
        return FAIL;
    }

    // Desalocar a lista antiga
    if (folha->conteudo.noFolha->lista != NULL) {
        dDDLL(&folha->conteudo.noFolha->lista);
    }

    // Substituir pela nova lista
    folha->conteudo.noFolha->lista = novaLista;

    // Opcionalmente, balancear a WebList
    balancearWL(pWL);

    return SUCCESS;
}


//Remove a lista de um nó folha e retorna-a.
int rmLista(pweblist pWL, int chave, ppDDLL rmLista) {
    if (pWL == NULL || rmLista == NULL) {
        return FAIL;
    }

    // Encontrar a folha com a chave especificada
    pweblist folha = encontrarFolhaComChave(pWL, chave);
    if (folha == NULL || folha->conteudo.noFolha == NULL) {
        return FAIL;
    }

    // Obter a lista a ser removida
    *rmLista = folha->conteudo.noFolha->lista;

    // Remover a referência à lista na folha
    folha->conteudo.noFolha->lista = NULL;

    // Opcionalmente, criar uma nova lista vazia na folha
    if (cDDLL(&folha->conteudo.noFolha->lista, pWL->sizedata) != SUCCESS) {
        return FAIL;  // Falha ao criar nova DDLL
    }

    // Opcionalmente, balancear a WebList
    balancearWL(pWL);

    return SUCCESS;
}

//Cria uma nova lista vazia em um nó folha específico, substituindo a lista existente.
int nvLista(pweblist pWL, int chave) {
    if (pWL == NULL) {
        return FAIL;
    }

    // Encontrar a folha com a chave especificada
    pweblist folha = encontrarFolhaComChave(pWL, chave);
    if (folha == NULL || folha->conteudo.noFolha == NULL) {
        printf("A folha com a chave %d não foi encontrada.\n", chave);
        return FAIL;
    }

    // Se já existe uma lista, destruí-la
    if (folha->conteudo.noFolha->lista != NULL) {
        int resultado = dDDLL(&(folha->conteudo.noFolha->lista));
        if (resultado != SUCCESS) {
            printf("Falha ao destruir a lista existente na folha com a chave %d.\n", chave);
            return FAIL;
        }
        folha->conteudo.noFolha->lista = NULL; // Certificar-se de que o ponteiro está nulo
    }

    // Criar uma nova DDLL vazia
    if (cDDLL(&(folha->conteudo.noFolha->lista), pWL->sizedata) != SUCCESS) {
        printf("Falha ao criar a nova DDLL na folha com a chave %d.\n", chave);
        return FAIL;
    }
    
    // Opcionalmente, balancear a WebList
    balancearWL(pWL);

    return SUCCESS;
}