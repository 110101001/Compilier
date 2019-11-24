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

IRVar *newVaribleIRVar(char *name){
	IRVar *newUnit=malloc(sizeof(IRVar));
	newUnit->type=VARIABLE;
	newUnit->name =name;
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
	newUnit->type=LABEL;
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

void removeNextStmt(IRStmtList* current){
	if(current->next==0){
		return;
	}
	IRStmtList *next=current->next->next;
	free(current->next);
	current->next=next;
	return;
}

char *printArg(IRVar *arg){
	if(arg==NULL){
		return NULL;
	}
	char *strArg=malloc(100*sizeof(char));
	switch(arg->type){
		case VARIABLE:
			strcp(strArg,arg->name);
			break;
		case LABEL:
			sprintf(strArg,"L%d",arg->no);
			break;
		case TEMP:
			sprintf(strArg,"_T%d",arg->no);
			break;
		case CONSTANT:
			sprintf(strArg,"#%d",arg->val);
			break;
	}
	return strArg;
}

void printLine(IRStmt *stmt){
	char str[100];
	char *arg1,*arg2,*target;
	arg1=printArg(stmt->arg1);
	arg2=printArg(stmt->arg2);
	target=printArg(stmt->target);
	switch(stmt->type){
		case _LABE:
			sprintf(str,"LABEL %s :",arg1);
			break;
		case _FUNC:
			sprintf(str,"FUNCTION %s :",arg1);
			break;
		case _ASSI:
			sprintf(str,"%s := %s",target,arg1);
			break;
		case _ADD:
			sprintf(str,"%s := %s + %s",target,arg1,arg2);
			break;
		case _SUB:
			sprintf(str,"%s := %s - %s",target,arg1,arg2);
			break;
		case _MULT:
			sprintf(str,"%s := %s * %s",target,arg1,arg2);
			break;
		case _DIVI:
			sprintf(str,"%s := %s / %s",target,arg1,arg2);
			break;
		case _ADDR:

			break;
		case _REFE:

			break;
		case _GOTO:
			sprintf(str,"GOTO %s",target);
			break;
		case _RETU:
			sprintf(str,"RETURN %s",arg1);
			break;
		case _DEC:
			sprintf(str,"DEC %s %s",target,arg1);
			break;
		case _ARG:
			sprintf(str,"ARG %s",arg1);
			break;
		case _CALL:
			sprintf(str,"%s := CALL %s",target,arg1);
			break;
		case _PARA:
			sprintf(str,"PARAM %s",target);
			break;
		case _READ:

			break;
		case _WRIT:

			break;
		case _IFL:
			sprintf(str,"if %s>%s GOTO %s",arg1,arg2,target);
			break;
		case _IFLE:
			sprintf(str,"if %s>=%s GOTO %s",arg1,arg2,target);
			break;
		case _IFE:
			sprintf(str,"if %s==%s GOTO %s",arg1,arg2,target);
			break;
		case _IFNE:
			sprintf(str,"if %s!=%s GOTO %s",arg1,arg2,target);
			break;
		case _IFS:
			sprintf(str,"if %s<%s GOTO %s",arg1,arg2,target);
			break;
		case _IFSE:
			sprintf(str,"if %s<=%s GOTO %s",arg1,arg2,target);
			break;


	}
	free(arg1);
	free(arg2);
	free(target);
	printf("%s\n",str);
	return;
}

void printCode(IRStmtList *head){
	IRStmtList *p=head;
	while(p!=0){
		printLine(p->stmt);
		p=p->next;
	}
}
