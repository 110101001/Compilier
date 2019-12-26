#ifndef __REGASSIGN_H_
#define __REGASSIGN_H_
#include "machine.h"

#define NEWBLOCK (block)malloc(sizeof(struct _block))
#define GETBIT(bv,n) (bv[(n)/8]&(1<<(n)%8))

#define MAXSREG 8//t0-t7 ,t8,t9 reserved for temp vars
#define MAXLREG 8//s0-s7, s8 for fp
#define MAXCOLOR MAXSREG+MAXLREG

struct _regDesc{
    int len;
    IRVar **var;
};
typedef struct _regDesc* regDesc;

struct _varDesc{
    int address;
    int regNum;
};
typedef struct _varDesc* varDesc;

struct _graphNeibor{
    struct _graphNode *node;
    struct _graphNeibor *next;
};
typedef struct _graphNeibor *graphNeibor;

struct _graphNode{
    IRVar *var;
    varDesc desc;
    graphNeibor neibor;
    enum{INGRAPH,POPED,OVERFLOWING} state;
    int crossCall;
};
typedef struct _graphNode* graphNode;

struct _block{
    IRStmtList *blockHeader;
    int len;
    IRVar **activeIn;
    IRVar **activeOut;
    struct _block *next1;
    struct _block *next2;
};
typedef struct _block* block;

typedef unsigned char bitVector;

int getReg(IRVar *var);
void resetReg(IRVar *var);
int getSymbolReg(IRVar *var);
block devideBlock(IRStmtList *head);

void funcActiveAnalyze(IRStmtList *head);
void graphColoring(IRStmtList *head);
int getMemoryPosition(IRVar *var,int size);
#endif
