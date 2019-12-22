#ifndef __REGASSIGN_H_
#define __REGASSIGN_H_
#include "machine.h"

#define NEWBLOCK (block)malloc(sizeof(struct _block))

struct _regDesc{
    int len;
    char **var;
};
typedef struct _regDesc* regDesc;

struct _block{
    IRStmtList *blockHeader;
    int len;
    IRVar *activeIn;
    IRVar *activeOut;
    struct _block *next1;
    struct _block *next2;
};
typedef struct _block* block;

block devideBlock(IRStmtList *head);

#endif
