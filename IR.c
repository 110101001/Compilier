#include "IR.h"
#include <malloc.h>
int currentNo;
int currentLabel;

IRStmtList *newStmtList(IRStmt *stmt){
	IRStmtList *newUnit=malloc(sizeof(IRStmtList));
	newUnit->stmt=stmt;
	newUnit->next=0;
	return newUnit;
}

IRStmt *newStmt(int type, IRVar *target,IRVar *arg1,IRVar *arg2){
	IRStmt *newUnit=malloc(sizeof(IRStmt));
	newUnit->type=type;
	newUnit->target=target;
	newUnit->arg1=arg1;
	newUnit->arg2=arg2;
	return newUnit;
}
IRVar *newTempIRVar(){
	IRVar *newUnit=malloc(sizeof(IRVar));
	newUnit->type=TEMP;
	newUnit->no=currentNo++;
	return newUnit;
}

IRVar *newVaribleIRVar(varibleItem *item){
	IRVar *newUnit=malloc(sizeof(IRVar));
	newUnit->type=VARIABLE;
	newUnit->name =item->name;
	return newUnit;
}
IRVar *newNumIRVar(int num){
	IRVar *newUnit=malloc(sizeof(IRVar));
	newUnit->type=CONSTANT;
	newUnit->val=num;
	return newUnit;
}

IRVar *newLabelIRVar(){
	IRVar *newUnit=malloc(sizeof(IRVar));
	newUnit->type=TEMP;
	newUnit->no=currentLabel++;
	return newUnit;
}
IRStmtList *catStmtList(IRStmtList *list1,IRStmtList *list2){
	IRStmtList *p1=list1;
	if(list1==NULL){
		return list2;
	}
	else if(list2==NULL){
		return list1;
	}
	while(p1->next!=0){
		p1=p1->next;
	}
	p1->next=list2;
	return list1;
}
