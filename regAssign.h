#ifndef __REGASSIGN_H_
#define __REGASSIGN_H_
#include "machine.h"

#define NEWBLOCK (block)malloc(sizeof(struct _block))

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

void resetReg(IRVar *var);
int getSymbolReg(IRVar *var);
block devideBlock(IRStmtList *head);

void funcActiveAnalyze(IRStmtList *head);
#endif
