#include "IR.h"
#include <malloc.h>
int currentNo;

IRStmtList *newStmtList(IRStmt *stmt){
	IRStmtList *newUnit=malloc(sizeof(IRStmtList));
	newUnit->stmt=stmt;
	newUnit->next=0;
	return newUnit;
}

IRStmt *newStmt(int type, IRVal *target,IRVal *arg1,IRVal *arg2){
	IRStmt *newUnit=malloc(sizeof(IRStmt));
	newUnit->type=type;
	newUnit->target=target;
	newUnit->arg1=arg1;
	newUnit->arg2=arg2;
	return newUnit;
}
IRVal *newTempIRVal(){
	IRVal *newUnit=malloc(sizeof(IRVal));
	newUnit->type=TEMP;
	newUnit->no=currentNo++;
	return newUnit;
}

IRVal *newVaribleIRVal(varibleItem *item){
	IRVal *newUnit=malloc(sizeof(IRVal));
	newUnit->type=VARIABLE;
	newUnit->name =item->name;
	return newUnit;
}
IRVal *newNumIRVal(int num){
	IRVal *newUnit=malloc(sizeof(IRVal));
	newUnit->type=CONSTANT;
	newUnit->val=num;
	return newUnit;
}
