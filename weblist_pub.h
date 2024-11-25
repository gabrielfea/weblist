# define SUCCESS 0
# define FAIL -1

typedef struct NoWebList *pNoWebList;
typedef struct weblist *pweblist, **ppweblist;
typedef struct DDLL *pDDLL, **ppDDLL;

// Funções operacionais

// Função: cWL
// Descrição:
//   Cria uma nova WebList com o nível especificado e tamanho de dados.
//   A WebList é uma estrutura hierárquica onde cada nó pode ter até 8 filhos,
//   formando uma árvore quaternária com profundidade definida por 'nivel'.
// Parâmetros:
//   - ppWL: ponteiro para ponteiro da WebList que será criada.
//   - nivel: profundidade da WebList (inteiro não negativo).
//   - sizedata: tamanho em bytes dos dados a serem armazenados em cada nó folha.
// Complexidade:
//   - O(8^n), onde n é o nível. Isso ocorre porque a função cWLRecursivo é chamada
//     recursivamente em cada um dos 8 filhos em cada nível, resultando em um total
//     de 8^n nós no nível n.
int cWL(ppweblist ppWL, int nivel, int sizedata);

// Função: dWL
// Descrição:
//   Destrói uma WebList previamente criada, liberando toda a memória alocada.
//   Percorre recursivamente todos os nós da árvore, liberando-os.
// Parâmetros:
//   - ppWL: ponteiro para ponteiro da WebList que será destruída.
// Complexidade:
//   - O(N), onde N é o número total de nós na WebList. A função dWLRecursivo percorre
//     toda a estrutura recursivamente, liberando cada nó individualmente.
int dWL(ppweblist ppWL);

// Funções focadas nos dados

// Função: iDado
// Descrição:
//   Insere um dado na WebList utilizando uma estratégia de distribuição baseada em 'ult_mov'.
//   O elemento é inserido no nó folha cujo identificador é determinado por 'ult_mov'.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - element: ponteiro para o elemento a ser inserido.
//   - cmp: função de comparação para elementos (não utilizada nesta função).
// Complexidade:
//   - O(N), onde N é o número total de nós na WebList. A função depende de 'encontrarFolhaComChave',
//     que percorre a árvore para encontrar o nó folha com a chave especificada. No pior caso,
//     pode ser necessário visitar todos os nós da árvore.
int iDado(pweblist pWL, void *element, int (*cmp)(void *, void *));

// Função: bDado
// Descrição:
//   Busca um dado na WebList.
//   Percorre todos os nós folha da WebList, procurando o elemento especificado.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - element: ponteiro para o elemento a ser buscado.
//   - cmp: função de comparação para elementos.
// Complexidade:
//   - O(N * M), onde N é o número de nós folha e M é o número médio de elementos
//     em cada lista dos nós folha. A função percorre todos os nós folha e, para cada um,
//     percorre sua lista de elementos até encontrar o elemento buscado.
int bDado(pweblist pWL, void *element, int (*cmp)(void *, void *));

// Função: rDado
// Descrição:
//   Remove um dado da WebList e ajusta a distribuição se necessário.
//   Após remover o elemento, pode ser necessário mover elementos entre nós folha para manter
//   o balanceamento da estrutura.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - element: ponteiro para o elemento a ser removido.
//   - cmp: função de comparação para elementos.
// Complexidade:
//   - O(N * M), onde N é o número de nós folha, M é o número médio de elementos em cada nó folha.
//     A função pode precisar percorrer todos os nós folha para encontrar o elemento.
//     Não precisa balancear depois de executada porque ela já remove cada elemento balanceando, puxando um elemento do fim.
int rDado(pweblist pWL, void *element, int (*cmp)(void *, void *));

// Função: pLista
// Descrição:
//   Imprime todos os elementos da WebList utilizando uma função de impressão personalizada.
//   Percorre recursivamente todos os nós folha e imprime os elementos de suas listas.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - printElement: função que imprime um elemento.
// Complexidade:
//   - O(N * M), onde N é o número de nós folha e M é o número médio de elementos em cada nó folha.
//     A função visita cada nó folha e percorre a lista de elementos para impressão.
int pLista(pweblist pWL, void (*printElement)(void *));

// Funções focadas nos nós folha

// Função: cpLista
// Descrição:
//   Copia a lista de um nó folha específico para uma nova lista.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - chave: identificador do nó folha cuja lista será copiada.
//   - retorno: ponteiro para a nova lista que receberá os dados.
// Complexidade:
//   - O(N + M), onde N é o número total de nós na WebList e M é o número de elementos no nó folha especificado.
//     A função precisa primeiro encontrar o nó folha (O(N)) e depois copiar os elementos da lista (O(M)).
int cpLista(pweblist pWL, int chave, ppDDLL retorno);

// Função: sbLista
// Descrição:
//   Substitui a lista de um nó folha por uma nova lista fornecida.
//   Após a substituição, pode ser necessário balancear a WebList.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - chave: identificador do nó folha cuja lista será substituída.
//   - novaLista: ponteiro para a nova lista que substituirá a existente.
// Complexidade:
//   - O(N + B), onde N é o número total de nós na WebList e B é o custo do balanceamento.
//     A função precisa encontrar o nó folha (O(N)) e depois realiza operações de balanceamento
//     com custo O(B), que será detalhado na função 'balancearWL'.
int sbLista(pweblist pWL, int chave, pDDLL novaLista);

// Função: rmLista
// Descrição:
//   Remove a lista de um nó folha e retorna-a.
//   A lista removida é armazenada em 'rmLista'.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - chave: identificador do nó folha cuja lista será removida.
//   - rmLista: ponteiro para onde a lista removida será armazenada.
// Complexidade:
//   - O(N + B), onde N é o número total de nós na WebList e B é o custo do balanceamento.
//     Semelhante à função 'sbLista', é necessário encontrar o nó folha e pode ser necessário
//     balancear a estrutura após a remoção.
int rmLista(pweblist pWL, int chave, ppDDLL rmLista);

// Função: nvLista
// Descrição:
//   Cria uma nova lista vazia em um nó folha específico, substituindo a lista existente.
//   Pode ser necessário balancear a WebList após essa operação.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - chave: identificador do nó folha onde a nova lista será criada.
// Complexidade:
//   - O(N + B), onde N é o número total de nós na WebList e B é o custo do balanceamento.
//     A função encontra o nó folha e substitui sua lista, podendo invocar o balanceamento.
int nvLista(pweblist pWL, int chave);

// Funções da WebList

// Função: nroNoFolha
// Descrição:
//   Retorna o número total de nós folha na WebList.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - retorno: ponteiro onde o resultado será armazenado.
// Complexidade:
//   - O(1), pois o número total de nós folha é armazenado diretamente na estrutura (g_key_counter).
int nroNoFolha(pweblist pWL, int *retorno);

// Função: nroEleNoFolha
// Descrição:
//   Retorna o número de elementos em um nó folha específico.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - chave: identificador do nó folha.
//   - retorno: ponteiro onde o resultado será armazenado.
// Complexidade:
//   - O(N + M), onde N é o número total de nós na WebList e M é o número de elementos no nó folha.
//     A função precisa encontrar o nó folha (O(N)) e depois contar os elementos em sua lista (O(M)).
int nroEleNoFolha(pweblist pWL, int chave, int *retorno);

// Função: nroEleWL
// Descrição:
//   Retorna o número total de elementos em toda a WebList.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - retorno: ponteiro onde o resultado será armazenado.
// Complexidade:
//   - O(N * M), onde N é o número de nós folha e M é o número médio de elementos em cada nó folha.
//     A função percorre todos os nós folha e soma o número de elementos em cada um.
int nroEleWL(pweblist pWL, int *retorno);

// Função: lstChaves
// Descrição:
//   Retorna uma lista com as chaves de todos os nós folha.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
//   - retorno: ponteiro para a lista que receberá as chaves.
// Complexidade:
//   - O(N), onde N é o número total de nós na WebList. A função percorre a árvore inteira para
//     coletar as chaves dos nós folha.
int lstChaves(pweblist pWL, ppDDLL retorno);

// Função: WLbalanceada
// Descrição:
//   Verifica se a WebList está balanceada em termos de distribuição de elementos nos nós folha.
//   Considera-se balanceada se cada nó folha tiver um número de elementos próximo da média,
//   com uma diferença máxima de 1.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
// Complexidade:
//   - O(N * M), onde N é o número de nós folha e M é o número médio de elementos em cada nó folha.
//     A função calcula o número total de elementos e verifica a distribuição em cada nó folha.
int WLbalanceada(pweblist pWL);

// Funções extras

// Função: balancearWL
// Descrição:
//   Realiza o balanceamento manual da WebList, redistribuindo os elementos entre os nós folha
//   para que todos tenham um número de elementos próximo da média.
// Parâmetros:
//   - pWL: ponteiro para a WebList.
// Complexidade:
//   - O(N^2 * M), onde N é o número de nós folha e M é o número médio de elementos em cada nó folha.
//     A função pode precisar comparar cada nó folha com todos os outros para redistribuir os elementos,
//     resultando em uma complexidade quadrática em relação ao número de nós folha.
int balancearWL(pweblist pWL);

// Função: printDDLL
// Descrição:
//   Imprime todos os elementos de uma lista DDLL utilizando uma função de impressão personalizada.
// Parâmetros:
//   - lista: ponteiro para a lista DDLL.
//   - printElement: função que imprime um elemento.
// Complexidade:
//   - O(M), onde M é o número de elementos na lista. A função percorre todos os elementos para impressão.
void printDDLL(pDDLL lista, void (*printElement)(void *));
