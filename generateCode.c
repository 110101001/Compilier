#include "IR.h"
#include "machine.h"
#include <stdio.h>
#include <malloc.h>

#define NEWCODE (code)malloc(sizeof(struct _code))
#define NEWOPERAND (operand)malloc(sizeof(struct _operand))
#define NEWFUNCSEG (funcSeg)malloc(sizeof(struct _funcSeg))

#define MAXTYPELEN 8
#define MAXDATALEN 100
#define MAXINSTRLEN 20
#define MAXOPERANDLEN 10

struct _dataItem retStr={
    _asciiz,"_ret",2,"\"\n\"",0
};

struct _dataItem promptStr={
    _asciiz,"_prompt",18,"\"Enter an integer:\"",&retStr
};

const dataItem standardDataSeg=&promptStr;

code getLast(code current){
	while(current->next!=NULL){
		current=current->next;
	}
	return current;
}

code catCode(code first,code second){
	if(first==NULL){
		return second;
	}
	code p=getLast(first);
	p->next=second;
	return first;
}

code generateOperand(operand op,IRVar *var){
	switch(var->type){
		case LABEL:
			op->type=_label;
			op->val=var->val;
			break;
		case VARIABLE:
		case TEMP:
			op->type=_reg;
			op->regNum=0;//TODO: implement getReg()
			break;
		case CONSTANT:
			op->type=_immi;
			op->val=var->val;
			break;
	}
	return NULL;
}

code generateCode(IRStmtList **head){
	IRStmtList *list=*head;
	code Code=NEWCODE;
	code last=Code;
	switch(list->stmt->type){
		case _LABE: 
			Code->instr=_label;
			Code->src1=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			break;
		case _FUNC: //UNREADCHABLE
			break;
		case _ASSI: 
			Code->instr=_move;
			Code->src1=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
		case _ADD: 
			if(list->stmt->arg2->type==CONSTANT){
				Code->instr=_addi;
			}
			else{
				Code->instr=_add;
			}
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->src2,list->stmt->arg2),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
		case _SUB: 
			Code->instr=_sub;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->src2,list->stmt->arg2),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
		case _MULT:
			Code->instr=_mul;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->src2,list->stmt->arg2),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
		case _DIVI: 
			Code->instr=_div;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->src2,list->stmt->arg2),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
		case _ADDR: 
			break;
		case _REFE: 
			break;
		case _WMEM:
			break;
		case _GOTO: 
			Code->instr=_j;
			Code->src1=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->target),Code);
			break;
		case _RETU: 
			break;
		case _DEC: 
			break;
		case _ARG: 
			break;
		case _CALL: 
			break;
		case _PARA: 
			break;
		case _READ:
			break;
		case _WRIT:
			break;
	case _IFL:
			Code->instr=_bgt;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->src2,list->stmt->arg2),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
	case _IFLE:
			Code->instr=_bge;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->src2,list->stmt->arg2),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
	case _IFS:
			Code->instr=_blt;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->src2,list->stmt->arg2),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
	case _IFSE:
			Code->instr=_ble;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->src2,list->stmt->arg2),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
	case _IFE:
			Code->instr=_beq;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->src2,list->stmt->arg2),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
	case _IFNE:
			Code->instr=_bne;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			Code=catCode(generateOperand(Code->src1,list->stmt->arg1),Code);
			Code=catCode(generateOperand(Code->src2,list->stmt->arg2),Code);
			Code=catCode(generateOperand(Code->dest,list->stmt->target),Code);
			break;
	}
	*head=list;
	return Code;
}

funcSeg generateFunc(IRStmtList **head){
	funcSeg func=NEWFUNCSEG;
	func->instrHead=NULL;
	func->next=NULL;
	func->funcName=(*head)->stmt->arg1->name;
	while((*head)->next!=0&&(*head)->next->stmt->type!=_FUNC){
		(*head)=(*head)->next;
		func->instrHead=catCode(func->instrHead,generateCode(head));
	}
	return func;
}

machineCode generateProgram(IRStmtList *head){
	IRStmtList **list=(IRStmtList **)malloc(sizeof(IRStmtList*));
	list=&head;
	machineCode MC=(machineCode)malloc(sizeof(struct _machineCode));
	MC->data=standardDataSeg;
	MC->func=NULL;
	funcSeg current;
	while((*list)->next!=NULL){
		if(MC->func==NULL){
			MC->func=generateFunc(list);
			current=MC->func;
		} 
		else{
			current->next=generateFunc(list);
			current=current->next;
		}
	}
	return MC;
}

char *printOperand(operand Operand){
	char *operandStr=malloc(MAXOPERANDLEN*sizeof(char));
	switch(Operand->type){
		case _immi:
			sprintf(operandStr,"%d",Operand->val);
			break;
		case _reg:
			sprintf(operandStr,"$%d",Operand->regNum);
			break;
		case _lab:
			sprintf(operandStr,"Label%d",Operand->val);
			break;
	}
	return operandStr;
}

char *printInstr(code Code){
	char *instr=malloc(MAXINSTRLEN*sizeof(char));
	char *src1,*src2,*dest;
	switch(Code->instr){
		case _add:
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"add %s, %s, %s",dest,src1,src2);
			free(src1);
			free(src2);
			free(dest);
			break;
		case _addi:	
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"addi %s, %s, %s",dest,src1,src2);
			free(src1);
			free(src2);
			free(dest);
			break;
		case _sub:
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"sub %s, %s, %s",dest,src1,src2);
			free(src1);
			free(src2);
			free(dest);
			break;
		case _mul:
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"mul %s, %s, %s",dest,src1,src2);
			free(src1);
			free(src2);
			free(dest);
			break;
		case _div:
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"div %s, %s, %s",dest,src1,src2);
			free(src1);
			free(src2);
			free(dest);
			break;
		case _move:
			src1=printOperand(Code->src1);
			dest=printOperand(Code->dest);
			sprintf(instr,"move %s, %s",dest,src1);
			free(src1);
			free(dest);
			break;
		case _lab:
			src1=printOperand(Code->src1);
			sprintf(instr,"%s:",src1);
			free(src1);
			break;
		case _j:
			src1=printOperand(Code->src1);
			sprintf(instr,"j %s",src1);
			free(src1);
			break;
		case _beq:
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"beq %s, %s, %s",src1,src2,dest);
			free(src1);
			free(src2);
			free(dest);
			break;
		case _bne:
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"bne %s, %s, %s",src1,src2,dest);
			free(src1);
			free(src2);
			free(dest);
			break;
		case _bgt:
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"bgt %s, %s, %s",src1,src2,dest);
			free(src1);
			free(src2);
			free(dest);
			break;
		case _blt:
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"blt %s, %s, %s",src1,src2,dest);
			free(src1);
			free(src2);
			free(dest);
			break;
		case _bge:
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"bge %s, %s, %s",src1,src2,dest);
			free(src1);
			free(src2);
			free(dest);
			break;
		case _ble:
			src1=printOperand(Code->src1);
			src2=printOperand(Code->src2);
			dest=printOperand(Code->dest);
			sprintf(instr,"ble %s, %s, %s",src1,src2,dest);
			free(src1);
			free(src2);
			free(dest);
			break;
	}
	return instr;
}

void printMachineCode(machineCode MC){
	dataItem item=MC->data;
	funcSeg func=MC->func;
	printf(".data\n");
	while(item!=NULL){
		char *typeStr=malloc(MAXTYPELEN*sizeof(char));
		char *dataStr=malloc(MAXDATALEN*sizeof(char));
		switch(item->type){
			case _asciiz:
				strcp(typeStr,".asciiz");
				strcp(dataStr,item->name);
				break;
			case _word:
				strcp(typeStr,".word");
				break;
			case _space:
				strcp(typeStr,".space");
				break;
		}
		printf("%s: %s %s\n",item->name,typeStr,dataStr);
		free(typeStr);
		free(dataStr);
		item=item->next;
	}
	printf(".global main\n");
	printf(".text\n");
	while(func!=NULL){
		printf("%s:\n",func->funcName);
		code instr=func->instrHead;
		while(instr!=NULL){
			char *instrStr=printInstr(instr);
			printf("%s\n",instrStr);
			free(instrStr);
			instr=instr->next;
		}
		func=func->next;
	}
}
