#include "machine.h"
#include "regAssign.h"

extern IRVar *self;

int getReg();
void blockDevide();
block devideBlock(IRStmtList *head){
    if(head==NULL){
        return NULL;
    }
    block b=NEWBLOCK;
    IRStmtList* p=head;
    b->len=0;
    while(
        p->next!=NULL
    &&p->next->stmt->type!=_LABE
    &&p->next->stmt->type!=_FUNC
    &&p->next->stmt->type!=_GOTO
    &&p->next->stmt->type<_IFL
    ){
        b->len++;
        p=p->next;
        //TODO: record active varible

    }
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