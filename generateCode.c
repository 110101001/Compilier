#include "IR.h"
#include "machine.h"
#include "regAssign.h"
#include <stdio.h>
#include <malloc.h>

const char regName[32][5]={
	"zero",
	"at",
	"v0","v1",
	"a0","a1","a2","a3",
	"t0","t1","t2","t3","t4","t5","t6","t7",
	"s0","s1","s2","s3","s4","s5","s6","s7",
	"t8","t9",
	"k0","k1",
	"gp",
	"sp",
	"fp",
	"ra"
};
#define NEWCODE (code)malloc(sizeof(struct _code))
#define NEWOPERAND (operand)malloc(sizeof(struct _operand))
#define NEWFUNCSEG (funcSeg)malloc(sizeof(struct _funcSeg))

#define MAXTYPELEN 8
#define MAXDATALEN 100
#define MAXINSTRLEN 20
#define MAXOPERANDLEN 10

extern int currentCount;
extern varDesc varLocation;
extern graphNode *nodes;

struct _dataItem retStr={
    _asciiz,"_ret",2,"\"\n\"",0
};

struct _dataItem promptStr={
    _asciiz,"_prompt",18,"\"Enter an integer:\"",&retStr
};
int argCount;
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

operand newOperand(operandType type,int n){
	operand op=NEWOPERAND;
	op->type=type;
	op->val=n;
	return op;
}
operand newRefeOperand(int offset,int baseReg){
	operand op=NEWOPERAND;
	op->type=_refe;
	op->baseRegNum=baseReg;
	op->offset=offset;
	return op;
}

code newCode(instrType instr,operand dest,operand src1,operand src2){
	code Code=NEWCODE;
	Code->instr=instr;
	Code->dest=dest;
	Code->src1=src1;
	Code->src2=src2;	
	Code->next=NULL;
	return Code;
}

code generateOperand(operand op,IRVar *var,int pos){
	code Code=NULL;
	switch(var->type){
		case LABEL:
			op->type=_label;
			op->val=var->val;
			break;
		case VARIABLE:
		case TEMP:
			op->type=_reg;
			op->regNum=getReg(var);
			if(op->regNum==-1){
				if(pos){
					Code=newCode(_lw,
					newOperand(_reg,$T8+pos-1),
					newRefeOperand(getMemoryPosition(var,4),$FP),
					NULL);
					op->regNum=Code->dest->regNum;
				}
				else{
					Code=newCode(_sw,
					newRefeOperand(getMemoryPosition(var,4),$FP),
					newOperand(_reg,$V0),
					NULL);
					op->regNum=Code->src1->regNum;
				}
			}
			break;
		case CONSTANT:
			op->type=_immi;
			op->val=var->val;
			break;
		case FUNCTION:
			op->type=_func;
			op->name=var->name;
			break;
	}
	return Code;
}

code generateCode(IRStmtList **head){
	IRStmtList *list=*head;
	code Code=NEWCODE;
	code retCode=Code;
	switch(list->stmt->type){
		case _LABE: 
			Code->instr=_label;
			Code->src1=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			break;
		case _FUNC: //UNREADCHABLE
			break;
		case _ASSI: 
			Code->instr=_move;
			Code->src1=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
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
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			break;
		case _SUB: 
			Code->instr=_sub;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			break;
		case _MULT:
			Code->instr=_mul;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			break;
		case _DIVI: 
			Code->instr=_div;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			Code=NEWCODE;
			Code->instr=_mflo;
			Code->dest=NEWOPERAND;
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
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
			retCode=catCode(generateOperand(Code->src1,list->stmt->target,0),retCode);
			break;
		case _RETU: 
			Code->instr=_move;
			Code->src1=NEWOPERAND;
			Code->dest=newOperand(_reg,$V0);
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			Code=newCode(_jr,NULL,newOperand(_reg,$RA),NULL);
			retCode=catCode(retCode,Code);
			break;
		case _DEC:
			break;
		case _ARG:
			if(argCount<4){
				Code->instr=_move;
				Code->src1=NEWOPERAND;
				Code->dest=newOperand(_reg,$A0+argCount);
				retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			}
			else{
				Code->instr=_move;
				Code->src1=NEWOPERAND;
				Code->dest=newRefeOperand(getAddress(4),$FP);
				retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			}
			argCount++;
			break;
		case _CALL: 
			argCount=0;
			Code->instr=_jal;
			Code->src1=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			Code=NEWCODE;
			Code->instr=_move;
			Code->src1=newOperand(_reg,$V0);
			Code->dest=NEWOPERAND;
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			retCode=stackLiveVar(list,retCode);
			break;
		case _PARA: 
			if(argCount<4){
				Code->instr=_move;
				Code->src1=NEWOPERAND;
				Code->dest=newOperand(_reg,$A0+argCount);
				retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			}
			else{
				Code->instr=_move;
				Code->src1=NEWOPERAND;
				Code->dest=newRefeOperand(getAddress(4),$FP);
				retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			}
			argCount++;
			break;
		case _READ:
			break;
		case _WRIT:
			free(Code);
			retCode=NULL;
			break;
	case _IFL:
			Code->instr=_bgt;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			generateOperand(Code->dest,list->stmt->target,0);
			break;
	case _IFLE:
			Code->instr=_bge;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			generateOperand(Code->dest,list->stmt->target,0);
			break;
	case _IFS:
			Code->instr=_blt;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			generateOperand(Code->dest,list->stmt->target,0);
			break;
	case _IFSE:
			Code->instr=_ble;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			generateOperand(Code->dest,list->stmt->target,0);
			break;
	case _IFE:
			Code->instr=_beq;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			generateOperand(Code->dest,list->stmt->target,0);
			break;
	case _IFNE:
			Code->instr=_bne;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			generateOperand(Code->dest,list->stmt->target,0);
			break;
	}
	*head=list;
	return retCode;
}

funcSeg generateFunc(IRStmtList **head){
	funcSeg func=NEWFUNCSEG;
	func->instrHead=NULL;
	func->next=NULL;
	func->funcName=(*head)->stmt->arg1->name;
	funcActiveAnalyze(*head);
	graphColoring(*head);
	while((*head)->next!=0&&(*head)->next->stmt->type!=_FUNC){
		(*head)=(*head)->next;
		func->instrHead=catCode(func->instrHead,generateCode(head));
	}
	return func;
}

machineCode generateProgram(IRStmtList *head){
	IRStmtList **list=(IRStmtList **)malloc(sizeof(IRStmtList*));
	varLocation=(varDesc)malloc(currentCount*sizeof(struct _varDesc));
	list=&head;
	/*while((*list)->stmt->type!=_FUNC||strcm((*list)->stmt->arg1->name,"main")){
		(*list)=(*list)->next;
	}*/
	//block b=devideBlock(*list);
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
			sprintf(operandStr,"$%s",regName[Operand->regNum]);
			break;
		case _lab:
			sprintf(operandStr,"Label%d",Operand->val);
			break;
		case _refe:
			sprintf(operandStr,"%d($%s)",Operand->offset,regName[Operand->baseRegNum]);
			break;
		case _func:
			sprintf(operandStr,"%s",Operand->name);
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
		case _sw:
			src1=printOperand(Code->src1);
			dest=printOperand(Code->dest);
			sprintf(instr,"sw %s, %s",dest,src1);
			free(src1);
			free(dest);
			break;
		case _lw:
			src1=printOperand(Code->src1);
			dest=printOperand(Code->dest);
			sprintf(instr,"lw %s, %s",dest,src1);
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
