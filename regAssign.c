#include "machine.h"
#include "regAssign.h"
#include "optimize.h"
#include <malloc.h>

extern IRVar *self;
varDesc varLocation;
int regCount;

int getReg();
void resetReg(IRVar *var){
    int pos=findVar(var);
    varLocation[pos].regNum=-1;
}
int getSymbolReg(IRVar *var){
    int pos=findVar(var);
    if(varLocation[pos].regNum==-1){
        varLocation[pos].regNum=regCount++;
    }
    return varLocation[pos].regNum;
}

block devideBlock(IRStmtList *head){
    if(head==NULL){
        return NULL;
    }
    if(head->blk!=NULL){
        return head->blk;
    }
    block b=NEWBLOCK;
    head->blk=b;
    b->blockHeader=head;
    IRStmtList* p=head->next;
    b->len=1;
    while(
        p!=NULL
    &&p->stmt->type!=_GOTO
    &&p->stmt->type<_IFL
    &&p->next!=NULL
    &&p->next->stmt->type!=_LABE
    ){
        b->len++;
        p->blk=b;
        p=p->next;
    }
    if(p!=NULL&&p->next!=NULL){
    switch(p->next->stmt->type){
        case _LABE:
            b->next1=devideBlock(p->next);
            break;
        case _GOTO:
            b->next1=devideBlock(searchLabel(p->stmt->target));
            break;
        case _IFL:
        case _IFLE:
        case _IFE:
        case _IFNE:
        case _IFSE:
        case _IFS:
            b->next1=devideBlock(p->next);
            b->next2=devideBlock(searchLabel(p->stmt->target));
            break;
    }
    }
    return b;
}