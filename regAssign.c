#include "machine.h"
#include "regAssign.h"
char **vars;

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
    &&p->next->stmt->type!=_CALL
    &&p->next->stmt->type<_IFL
    ){
        b->len++;
        p=p->next;
        //TODO: record active varible
    }
    //TODO: devide block for the last stmt's target
}