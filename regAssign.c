#include "machine.h"
#include "regAssign.h"
#include "optimize.h"
#include <malloc.h>

extern IRVar **Self;
extern int currentCount;
varDesc varLocation;
int regCount;

int getReg();
void resetReg(IRVar *var){
    int pos=findVar(Self,currentCount, var);
    varLocation[pos].regNum=-1;
}
int getSymbolReg(IRVar *var){
    int pos=findVar(Self,currentCount, var);
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

bitVector *createBV(int n){
    int len=n/8;
    return malloc(len*sizeof(bitVector));
}

void merge(int len,bitVector *a,bitVector *b){
    for(int i=0;i<len/8;i++){
        a[i]=a[i]|b[i];
    }
}

void funcActiveAnalyze(IRStmtList *head){
    //find all Vars
    IRVar **funcVars=(IRVar **)malloc(currentCount*sizeof(IRVar*));

		IRStmtList *p=head;
        int varCount=0;
		while(p->next!=NULL&&p->next->stmt->type!=_FUNC){
            p=p->next;
			if(p->stmt->target!=NULL&&(p->stmt->target->type==TEMP||p->stmt->target->type==VARIABLE)){
                int no=findVar(funcVars,varCount,p->stmt->target);
                if(no==-1){
                    funcVars[varCount++]=p->stmt->target;
                }
            }
			if(p->stmt->arg1!=NULL&&(p->stmt->arg1->type==TEMP||p->stmt->arg1->type==VARIABLE)){
                int no=findVar(funcVars,varCount,p->stmt->arg1);
                if(no==-1){
                    funcVars[varCount++]=p->stmt->arg1;
                }
            }
			if(p->stmt->arg2!=NULL&&(p->stmt->arg2->type==TEMP||p->stmt->arg2->type==VARIABLE)){
                int no=findVar(funcVars,varCount,p->stmt->arg2);
                if(no==-1){
                    funcVars[varCount++]=p->stmt->arg2;
                }
            }
        }
    //calc active vars in rev order
    int changeFlag=1;
    IRStmtList *pi=p;
    while(changeFlag==1){
        changeFlag=0;
        while(pi!=NULL&&pi->stmt->type!=_FUNC){
            pi->in=createBV(varCount);
            pi->out=createBV(varCount);
            //calc out
            if(pi->stmt->type>=_IFL){
                merge(varCount,pi->out,pi->next->in);
            }
            pi=pi->next;
        }
    }
    
}