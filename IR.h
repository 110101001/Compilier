#ifndef _IR_H_
#define _IR_H_

#include<stdio.h>
#include"symbolTable.h"

typedef struct IRVar{
	enum {VARIABLE,LABEL,TEMP,CONSTANT,ADDRESS} type;
	union{
		int val;
		int no;
		char *name;
	};
}IRVar;

typedef struct IRStmt{
	enum {_LABE, _FUNC, _ASSI , _ADD, _SUB, _MULT, _DIVI , _ADDR, _REFE, _WMEM ,_GOTO, _RETU, _DEC, _ARG, _CALL, _PARA, _READ ,_WRIT,
	_IFL,_IFLE,_IFS,_IFSE,_IFE,_IFNE} type;
	IRVar *target;
	IRVar *arg1;
	IRVar *arg2;
}IRStmt;

typedef struct IRStmtList{
	IRStmt *stmt;
	struct IRStmtList *next;
}IRStmtList;

IRStmtList *newStmtList(IRStmt *stmt);
IRStmt *newStmt(int type, IRVar *target,IRVar *arg1,IRVar *arg2);
IRVar *newTempIRVar();
IRVar *newVaribleIRVar(char *name);
IRVar *newNumIRVar(int num);
IRVar *newLabelIRVar();
IRStmtList *catStmtList(IRStmtList *list1,IRStmtList *list2);
void removeNextStmt(IRStmtList* current);
IRStmtList *getStmtListByLine(int n,IRStmtList *head);
void printCode(IRStmtList *head);

#endif
