#include "IROptimize.h"
#include <malloc.h>

extern int currentNo;

void IROptimize(IRStmtList *head){
	int *lastUse=malloc(sizeof(int)*currentNo);
	int *lastChange=malloc(sizeof(int)*currentNo);
	int line=0;
	IRStmtList *p=head;
	while(p!=0){
		if(p->stmt->target!=NULL&&p->stmt->target->type==TEMP){
			lastChange[p->stmt->target->no]==line;
		}
		if(p->stmt->arg1!=NULL&&p->stmt->arg1->type==TEMP){
			lastUse[p->stmt->arg1->no]==line;
		}
		if(p->stmt->arg2!=NULL&&p->stmt->arg2->type==TEMP){
			lastUse[p->stmt->arg2->no]==line;
		}
		line++;
		p=p->next;
	}
	for(int i=0;i<currentNo;i++){
		
	}
}
