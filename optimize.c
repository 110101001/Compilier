#include "optimize.h"
#include <malloc.h>
#include <stdlib.h>

extern int currentNo;
extern int varCount;
int currentCount;
IRVar **Self;

int findVar(IRVar *var){
	for(int i=0;i<currentCount;i++){
		if(Self[i]->type==var->type){
			if(Self[i]->val==var->val){
				return i;
			}
			else if(Self[i]->type==VARIABLE&&(strcm(Self[i]->name,var->name)==0)){
				return i;
			}
		}
	}
	return -1;
}

IRStmtList *IROptimize(IRStmtList *head){
	currentCount=0;
	Self=malloc(sizeof(IRVar *)*(currentNo+varCount));
	IRVar **lastAssigned=calloc(currentNo+varCount,sizeof(IRVar *));
	int *lastUse=calloc((currentNo+varCount),sizeof(int));
	int *lastChange=calloc((currentNo+varCount),sizeof(int));
	int line=0;
	IRStmtList *p=head;
	while(p!=0){
		if(p->stmt->target!=NULL&&(p->stmt->target->type==TEMP||p->stmt->target->type==VARIABLE)){
			int no=findVar(p->stmt->target);
			if(no==-1){
				no=currentCount;
				Self[currentCount++]=p->stmt->target;
			}
			if(p->stmt->type==_ASSI||p->stmt->type==_REFE||p->stmt->type==_ADDR){
				lastAssigned[no]=p->stmt->arg1;
			}
			lastChange[no]=line;
		}
		if(p->stmt->arg1!=NULL&&(p->stmt->arg1->type==TEMP||p->stmt->arg1->type==VARIABLE)){
			int no=findVar(p->stmt->arg1);
			if(no==-1){
				no=currentCount;
				Self[currentCount++]=p->stmt->arg1;
			}
			lastUse[no]=line;
		}
		if(p->stmt->arg2!=NULL&&(p->stmt->arg2->type==TEMP||p->stmt->arg2->type==VARIABLE)){
			int no=findVar(p->stmt->arg2);
			if(no==-1){
				no=currentCount;
				Self[currentCount++]=p->stmt->arg2;
			}
			lastUse[no]=line;
		}
		line++;
		p=p->next;
	}
	for(int i=0;i<currentCount;i++){
		char *str;
		str=printArg(Self[i]);
		//printf("%s:last use:%d,last change:%d\n",str,lastUse[i],lastChange[i]);
		free(str);
		if(lastUse[i]==0){
			//printf("Can delete\n");	
			delIRVar(Self[i],head);
		}
		else if(lastChange[i]>lastUse[i]){
			//printf("Can delete some stmt\n");
		}
		if(lastAssigned[i]!=0){
			if(lastAssigned[i]->type==TEMP){
				IRVar *dupVar=lastAssigned[i];
				if(dupVar->type==CONSTANT||lastChange[dupVar->no]<lastChange[i]){
					//str=printArg(lastAssigned[i]);
					//printf("Can merge with %s\n",str);
					//free(str);
					IRStmtList *p;
					p=getStmtListByLine(lastChange[i],head);
					if(p->stmt->type==_ASSI){
						replaceIRVar(Self[i],dupVar,head);
						removeStmt(p);
					}
				}
			}
		}
	}
	head=doRemove(head);

	return head;
}
