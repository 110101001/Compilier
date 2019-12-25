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
	int len=(n-1)/8+1;
	return calloc(len,sizeof(bitVector));
}

void merge(int len,bitVector **a,bitVector *b){
	if(*a==NULL){
		*a=createBV(len);
	}
	if(b==NULL){
		return;
	}
	for(int i=0;i<(len-1)/8+1;i++){
		(*a)[i]=(*a)[i]|b[i];
	}
}

void resetBit(int n,bitVector *a){
	a[n/8]&=~(1<<n%8);
}

void setBit(int n,bitVector *a){
	a[n/8]|=(1<<n%8);
}

int compareFlow(int n,bitVector *a,bitVector *b){
	for(int i=0;i<(n-1)/8+1;i++){
		if(a[i]!=b[i]){
			return 1;
		}
	}
	return 0;
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
	while(changeFlag==1){
		changeFlag=0;
	    IRStmtList *pi=p;
		while(pi!=NULL&&pi->stmt->type!=_FUNC){
			bitVector *tempIn=NULL;
			bitVector *tempOut=NULL;
			merge(varCount,&tempIn,pi->in);
			merge(varCount,&tempOut,pi->out);
			//calc out
			merge(varCount,&pi->out,NULL);
			if(pi->stmt->type==_GOTO){
                IRStmtList *lab=searchLabel(pi->stmt->target);
				merge(varCount,&pi->out,lab->in);
			}
			else{
				if(pi->stmt->type>=_IFL){
                    IRStmtList *lab=searchLabel(pi->stmt->target);
					merge(varCount,&pi->out,lab->in);
				}
				if(pi->next!=NULL&&pi->next->stmt->type!=_FUNC){
					merge(varCount,&pi->out,pi->next->in);
				}
			}
			//calc in
			merge(varCount,&pi->in,pi->out);
			if(pi->stmt->target!=NULL&&
					(pi->stmt->target->type==TEMP||pi->stmt->target->type==VARIABLE)&&
					!(
						pi->stmt->type==_WMEM
					 )){
				resetBit(findVar(funcVars,varCount,pi->stmt->target),pi->in);
			}
			if(pi->stmt->arg1!=NULL&&
					(pi->stmt->arg1->type==TEMP||pi->stmt->arg1->type==VARIABLE)&&
					!(
						0
					 )){
				setBit(findVar(funcVars,varCount,pi->stmt->arg1),pi->in);
			}
			if(pi->stmt->arg2!=NULL&&
					(pi->stmt->arg2->type==TEMP||pi->stmt->arg2->type==VARIABLE)&&
					!(
						0
					 )){
				setBit(findVar(funcVars,varCount,pi->stmt->arg2),pi->in);
			}
			if(compareFlow(varCount,tempIn,pi->in)){
				changeFlag=1;
			}
			if(compareFlow(varCount,tempOut,pi->out)){
				changeFlag=1;
			}
			free(tempIn);
			free(tempOut);
			pi=pi->prev;
		}
	}

}
