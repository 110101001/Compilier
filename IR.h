#ifndef _IR_H_
#define _IR_H_

#include<stdio.h>
#include"symbolTable.h"

typedef struct IRVar{
	enum {VARIABLE,LABEL,TEMP,CONSTANT,ADDRESS,FUNCTION,SIZE} type;
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
	int removeFlag;
	struct IRStmtList *next;
}IRStmtList;

IRStmtList *newStmtList(IRStmt *stmt);
IRStmt *newStmt(int type, IRVar *target,IRVar *arg1,IRVar *arg2);
IRVar *newTempIRVar();
IRVar *newSizeIRVar(int n);
IRVar *newVaribleIRVar(char *name);
IRVar *newFunctionIRVar(char *name);
IRVar *newNumIRVar(int num);
IRVar *newLabelIRVar();
int cmpIRVar(IRVar *a,IRVar *b);
IRStmtList *catStmtList(IRStmtList *list1,IRStmtList *list2);
void replaceIRVar(IRVar *var,IRVar *newVar,IRStmtList *head);
void delIRVar(IRVar *var,IRStmtList *head);
void removeStmt(IRStmtList* current);
void removeNextStmt(IRStmtList* current);
IRStmtList *doRemove(IRStmtList *head);
IRStmtList *getStmtListByLine(int n,IRStmtList *head);
char *printArg(IRVar *arg);
void printLine(IRStmt *stmt);
void printCode(IRStmtList *head);

#endif
