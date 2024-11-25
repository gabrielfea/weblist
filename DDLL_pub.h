#define SUCCESS 0
#define FAIL -1

typedef struct DDLL *pDDLL, **ppDDLL;

// Funções básicas de uma DDLL (DISPONÍVEIS)
int cDDLL(ppDDLL pp, int sizedata);
int dDDLL(ppDDLL pp);
int cleanDDLL(pDDLL p);

int iBegin(pDDLL p, void *element);
int iEnd(pDDLL p, void *element);
int iPosition(pDDLL p, int N, void *element);

int rBegin(pDDLL p, void *element);
int rEnd(pDDLL p, void *element);
int rPosition(pDDLL p, int N, void *element);

int sBegin(pDDLL p, void *element);
int sEnd(pDDLL p, void *element);
int sPosition(pDDLL p, int N, void *element);

// Funções adicionais
int empty(pDDLL p);
int countElements(pDDLL p);