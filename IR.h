#ifndef _IR_H_
#define _IR_H_

#include<stdio.h>
#include"symbolTable.h"

typedef struct IRVal{
	enum {VARIABLE,TEMP,CONSTANT,ADDRESS} type;
	union{
		int val;
		int no;
		char *name;
	};
}IRVal;

typedef struct IRStmt{
	enum code{_LABE, _FUNC, _ASSI , _ADD, _SUB, _MULT, _DIVI , _ADDR, _REFE, _GOTO, _IF, _RETU, _DEC, _ARG, _CALL, _PARA, _READ ,_WRIT} type;
	IRVal *target;
	IRVal *arg1;
	IRVal *arg2;
}IRStmt;

typedef struct IRStmtList{
	IRStmt *stmt;
	struct IRStmtList *next;
}IRStmtList;

IRStmtList *newStmtList(IRStmt *stmt);
IRStmt *newStmt(int type, IRVal *target,IRVal *arg1,IRVal *arg2);
IRVal *newTempIRVal();
IRVal *newVaribleIRVal(varibleItem *item);
IRVal *newNumIRVal(int num);

#endif
