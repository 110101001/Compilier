#ifndef __REGASSIGN_H_
#define __REGASSIGN_H_
#include "machine.h"

#define NEWBLOCK (block)malloc(sizeof(struct _block))

typedef struct blockList{
    block b;
    struct blockList* next;
}blockList;

struct _block{
    IRStmtList *blockHeader;
    int len;
    int *activeIn;
    int *activeOut;
    struct _block *next;
};
typedef struct _block* block;

block devideBlock(IRStmtList *head);

#endif
