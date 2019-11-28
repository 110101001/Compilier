#include "optimize.h"
#include <malloc.h>
#include <stdlib.h>

extern int currentNo;
extern int varCount;
int currentCount;
IRVar **Self;

int findVar(IRVar *var){
	for(int i=0;i<currentCount;i++){
		if(cmpIRVar(var,Self[i])){
			return i;
		}
	}
	return -1;
}
//代码执行的顺序可能和它本身的顺序毫无关系，因此试图仅靠变量在代码中的位置来确定是否可以用一个变量替代另一个变量以期省略一个赋值语句的操作是必定存在隐患的。
IRStmtList *IROptimize(IRStmtList *head){
	int changedFlag=1;
	while(changedFlag==1){
		currentCount=0;
		changedFlag=0;
		Self=malloc(sizeof(IRVar *)*(currentNo+varCount));
		IRVar **lastAssigned=calloc(currentNo+varCount,sizeof(IRVar *));
		int *lastUse=calloc((currentNo+varCount),sizeof(int));
		int *firstUse=calloc((currentNo+varCount),sizeof(int));
		int *firstChange=calloc((currentNo+varCount),sizeof(int));
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
				if(firstChange[no]==0){
					firstChange[no]=line;
				}
			}
			if(p->stmt->arg1!=NULL&&(p->stmt->arg1->type==TEMP||p->stmt->arg1->type==VARIABLE)){
				int no=findVar(p->stmt->arg1);
				if(no==-1){
					no=currentCount;
					Self[currentCount++]=p->stmt->arg1;
				}
				lastUse[no]=line;
				if(firstUse[no]==0){
					firstUse[no]=line;
				}
			}
			if(p->stmt->arg2!=NULL&&(p->stmt->arg2->type==TEMP||p->stmt->arg2->type==VARIABLE)){
				int no=findVar(p->stmt->arg2);
				if(no==-1){
					no=currentCount;
					Self[currentCount++]=p->stmt->arg2;
				}
				lastUse[no]=line;
				if(firstUse[no]==0){
					firstUse[no]=line;
				}
			}
			line++;
			p=p->next;
		}
		for(int i=0;i<currentCount;i++){
			//char *str,*str2;
			//str=printArg(Self[i]);
			//printf("%s:last use:%d,last change:%d\n",str,lastUse[i],lastChange[i]);
			if(lastUse[i]==0){
				delIRVar(Self[i],head);
				changedFlag=1;
				//printf("Deleted stmt about %s\n",str);
			}
			/*if(lastAssigned[i]!=0){
				if(lastAssigned[i]->type==CONSTANT){
					IRVar *dupVar=lastAssigned[i];
					str2=printArg(dupVar);
					IRStmtList *p;
					p=getStmtListByLine(lastChange[i],head);
					if(p->stmt->type==_ASSI){
						replaceIRVar(Self[i],dupVar,p);
						removeStmt(p);
						printf("Remove line ");
						printLine(p->stmt);
						printf("Merge %s with %s\n",str,str2);	
						changedFlag=1;
					}
					free(str2);
				}
			}*/
			//free(str);
		}
		head=doRemove(head);
	}
	return head;
}
