#include "optimize.h"
#include <malloc.h>
#include <stdlib.h>

extern int currentNo;
extern int varCount;
extern int currentLabel;
int currentCount;
IRVar **Self;

int findVar(IRVar **list,int len,IRVar *var){
	for(int i=0;i<len;i++){
		if(cmpIRVar(var,list[i])){
			return i;
		}
	}
	return -1;
}
//����ִ�е�˳����ܺ���������˳����޹�ϵ�������ͼ���������ڴ����е�λ����ȷ���Ƿ������һ�����������һ����������ʡ��һ����ֵ���Ĳ����Ǳض����������ġ���ˣ���ֹ��������Ĳ�������translater��ͷ��
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
				int no=findVar(Self,currentCount,p->stmt->target);
				if(no==-1){
					no=currentCount;
					Self[currentCount++]=p->stmt->target;
				}
				if(p->stmt->type==_ASSI||p->stmt->type==_REFE||p->stmt->type==_ADDR){
					lastAssigned[no]=p->stmt->arg1;
				}
				lastChange[no]=line;
				if(p->stmt->type==_CALL){
					lastUse[no]=line;
				}
				if(firstChange[no]==0){
					firstChange[no]=line;
				}
			}
			if(p->stmt->arg1!=NULL&&(p->stmt->arg1->type==TEMP||p->stmt->arg1->type==VARIABLE)){
				int no=findVar(Self,currentCount,p->stmt->arg1);
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
				int no=findVar(Self,currentCount,p->stmt->arg2);
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
		for(int i=0;i<currentCount;i++){//Delete unused var
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

		int *used=calloc(currentLabel,sizeof(int));
		p=head;
		while(p!=0){
			if(p->stmt->type==_GOTO||
					(p->stmt->type>=_IFL&&p->stmt->type<=_IFNE)
					){//count label use
				used[p->stmt->target->no]+=1;
			}
			if(p->stmt->type==_RETU&&
					p->next!=0&&
					(p->next->stmt->type!=_FUNC&&
					 p->next->stmt->type!=_LABE)
			  ){//Delete instr after return
				removeStmt(p->next);
				if(p->stmt->type==_GOTO){
					used[p->stmt->target->no]-=1;
				}
				changedFlag=1;
			}
			if(p->stmt->type==_GOTO&&
					p->next!=0&&
					p->next->stmt->type==_LABE&&
					cmpIRVar(p->stmt->target,p->next->stmt->arg1)
			  ){//Delete goto which goes to next instr
				removeStmt(p);
				used[p->stmt->target->no]-=1;
				changedFlag=1;
			}
			if(p->stmt->type==_LABE&&
					p->next!=0&&
					p->next->stmt->type==_LABE&&
					cmpIRVar(p->stmt->arg1,p->next->stmt->arg1)==0
			  ){//Merge two labels
				replaceIRVar(p->stmt->arg1,p->next->stmt->arg1,head);	
				used[p->next->stmt->arg1->no]+=used[p->stmt->arg1->no];
				removeStmt(p);
				changedFlag=1;
			}
			p=p->next;
		}
		p=head;
		while(p!=0){
			if(p->stmt->type==_LABE&&
				used[p->stmt->arg1->no]==0
			  ){
				removeStmt(p);
				changedFlag=1;
			}
			p=p->next;
		}
		head=doRemove(head);
	}
	return head;
}
