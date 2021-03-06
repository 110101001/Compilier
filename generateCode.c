#include "IR.h"
#include "machine.h"
#include "regAssign.h"
#include <stdio.h>
#include <malloc.h>

const char regName[32][5]={
	"0",
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

const char builtInFunc[]=
"\
read:\n\
  li $v0, 4\n\
  la $a0, _prompt\n\
  syscall\n\
  li $v0, 5\n\
  syscall\n\
  jr $ra\n\
  \n\
write:\n\
  li $v0, 1\n\
  syscall\n\
  li $v0, 4\n\
  la $a0, _ret\n\
  syscall\n\
  move $v0, $0\n\
  jr $ra\n\
  \n"
;
#define NEWCODE (code)malloc(sizeof(struct _code))
#define NEWOPERAND (operand)malloc(sizeof(struct _operand))
#define NEWFUNCSEG (funcSeg)malloc(sizeof(struct _funcSeg))

#define MAXTYPELEN 8
#define MAXDATALEN 100
#define MAXINSTRLEN 20
#define MAXOPERANDLEN 10

extern int currentCount;
extern int varCount;
extern int fpOffSet;
extern varDesc varLocation;
extern graphNode *nodes;

int loadCount;
int regAdr;

struct _dataItem retStr={
	_asciiz,"_ret",2,"\"\n\"",0
};

struct _dataItem promptStr={
	_asciiz,"_prompt",18,"\"Enter an integer:\"",&retStr
};
const dataItem standardDataSeg=&promptStr;

char *printInstr(code Code);

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

code removeCode(code head,code removed){
	code p=head;
	if(head==removed){
		head=removed->next;
		free(removed);
		return head;
	}
	while(p->next!=NULL&&p->next!=removed){
		p=p->next;
	}
	if(p->next==removed){
		p->next=removed->next;
		free(removed);
	}
	return head;
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
//0:dest,1:src1,2:src2,3:src1(reg only),4:src2(reg only)
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
			if(var->val==0){
				op->type=_reg;
				op->regNum=$ZERO;
			}
			else{
				if(pos>2){
					Code=newCode(_li,
							newOperand(_reg,$T8+pos-3),
							newOperand(_immi,var->val),
							NULL);
					op->type=_reg;
					op->regNum=Code->dest->regNum;
				}
				else{
					op->type=_immi;
					op->val=var->val;
				}
			}
			break;
		case FUNCTION:
			op->type=_func;
			op->name=var->name;
			break;
	}
	return Code;
}

code callStack(IRStmtList *head,code Code){
	code retCode=Code;
	code recoverCode=NULL;
	code varStack=NULL;
	bitVector *temp=NULL;
	merge(varCount,&temp,head->in);
	common(varCount,&temp,head->out);
	for(int i=0;i<varCount;i++){
		if(GETBIT(temp,i)&&
				nodes[i]->crossCall==1&&
				nodes[i]->desc->regNum>=MAXSREG){
			varStack=catCode(
					newCode(_sw,
						newRefeOperand(getAddress(4),$FP),
						newOperand(_reg,getReg(nodes[i]->var)),
						NULL),
					varStack);
			recoverCode=catCode(
					recoverCode,
					newCode(_lw,
						newOperand(_reg,getReg(nodes[i]->var)),
						newRefeOperand(getAddress(0)-4,$FP),
						NULL));
		}
	}
	free(temp);
	IRStmtList *p=head;
	int argCount=0;
	int totalArgCount=0;
	while(p->prev->stmt->type==_ARG){
		totalArgCount++;
		p=p->prev;
	}
	int addr;
	if(totalArgCount>4){
		getAddress(4*(totalArgCount-4));
		addr=getAddress(0);
	}
	p=head->prev;
	while(p->stmt->type==_ARG){
		if(argCount<4){
			Code=NEWCODE;
			Code->instr=_move;
			Code->src1=NEWOPERAND;
			Code->dest=newOperand(_reg,$A0+argCount);
			varStack=catCode(Code,varStack);
			varStack=catCode(generateOperand(Code->src1,p->stmt->arg1,1),varStack);
			if(Code->src1->type==_immi){
				Code->instr=_li;
			}
		}
		if(argCount>=4){
			Code=NEWCODE;
			Code->instr=_sw;
			Code->src1=NEWOPERAND;
			Code->dest=newRefeOperand(addr-4*(argCount-3),$FP);
			varStack=catCode(Code,varStack);
			varStack=catCode(generateOperand(Code->src1,p->stmt->arg1,3),varStack);
		}
		argCount++;
		p=p->prev;
	}
	
	varStack=catCode(
			varStack,
			newCode(_addi,
				newOperand(_reg,$SP),
				newOperand(_reg,$SP),
				newOperand(_immi,getAddress(0))));
	retCode=catCode(varStack,retCode);
	retCode=catCode(
			retCode,
			newCode(_sub,
				newOperand(_reg,$SP),
				newOperand(_reg,$SP),
				newOperand(_immi,getAddress(0))));
	
	retCode=catCode(retCode,recoverCode);
	return retCode;
}

code funcStart(){
	code Code;
	code retCode=NULL;
	retCode=catCode(
			retCode,
			newCode(_sw,
				newRefeOperand(getAddress(4),$SP),
				newOperand(_reg,$RA),
				NULL));
	retCode=catCode(
			retCode,
			newCode(_sw,
				newRefeOperand(getAddress(4),$SP),
				newOperand(_reg,$FP),
				NULL));
	//set fp
	Code=newCode(_move,
	newOperand(_reg,$FP),
	newOperand(_reg,$SP),
	NULL);
	retCode=catCode(retCode,Code);
	//save regs
	bitVector *lreg=createBV(MAXLREG);
	for(int i=0;i<varCount;i++){
		if(nodes[i]->desc->regNum>=MAXSREG){
			setBit(i,lreg);
		}
	}
	for(int i=0;i<MAXLREG;i++){
		if(GETBIT(lreg,i)){
			retCode=catCode(retCode,
			newCode(_sw,
				newRefeOperand(getAddress(4),$FP),
				newOperand(_reg,$S0+i),
				NULL)
			);
		}
	}
	regAdr=getAddress(0);
	
	free(lreg);
	return retCode;
}

code funcEnd(){
	code retCode=NULL;
	bitVector *lreg=createBV(MAXLREG);
	for(int i=0;i<varCount;i++){
		if(nodes[i]->desc->regNum>=MAXSREG){
			setBit(i,lreg);
		}
	}
	int regCount=1;
	for(int i=MAXLREG-1;i>=0;i--){
		if(GETBIT(lreg,i)){
			retCode=catCode(retCode,
			newCode(_lw,
				newOperand(_reg,$S0+i),
				newRefeOperand(regAdr-4*regCount,$FP),
				NULL)
			);
			regCount++;
		}
	}
	retCode=catCode(
			retCode,
			newCode(_lw,
				newOperand(_reg,$RA),
				newRefeOperand(0,$FP),
				NULL));
	retCode=catCode(
			retCode,
			newCode(_lw,
				newOperand(_reg,$FP),
				newRefeOperand(4,$FP),
				NULL));
	free(lreg);
	return retCode;
}
code generateCode(IRStmtList **head){
	IRStmtList *list=*head;
	code Code=NEWCODE;
	Code->next=NULL;
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
			retCode==NULL;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			if(Code->src1->type==_immi){
				Code->instr=_li;
			}
			if(Code->src1->type==_reg&&
			Code->dest->type==_reg&&
			Code->src1->regNum==Code->dest->regNum
			){
				retCode=removeCode(retCode,Code);
			}
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
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,3),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,2),retCode);
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			break;
		case _MULT:
			Code->instr=_mul;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,3),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,4),retCode);
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			break;
		case _DIVI: 
			Code->instr=_div;
			Code->src1=NEWOPERAND;
			Code->src2=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,3),retCode);
			retCode=catCode(generateOperand(Code->src2,list->stmt->arg2,4),retCode);
			Code=NEWCODE;
			Code->instr=_mflo;
			Code->dest=NEWOPERAND;
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			break;
		case _ADDR:
			Code->instr=_addi;
			Code->src1=newOperand(_reg,$FP);
			Code->src2=newOperand(_immi,getOffset(list->stmt->arg1));
			Code->dest=NEWOPERAND;
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			break;
		case _REFE: 
			Code->instr=_lw;
			Code->src1=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,1),retCode);
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->target,0));
			{
				int tempRegNum=Code->src1->regNum;
				Code->src1->type=_refe;
				Code->src1->baseRegNum=tempRegNum;
				Code->src1->offset=0;
			}
			break;
		case _WMEM:
			Code->instr=_sw;
			Code->src1=NEWOPERAND;
			Code->dest=NEWOPERAND;
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg2,3),retCode);
			retCode=catCode(retCode,generateOperand(Code->dest,list->stmt->arg1,0));
			{
				int tempRegNum=Code->dest->regNum;
				Code->dest->type=_refe;
				Code->dest->baseRegNum=tempRegNum;
				Code->dest->offset=0;
			}
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
			retCode=catCode(generateOperand(Code->src1,list->stmt->arg1,3),retCode);
			retCode=catCode(retCode,funcEnd());
			Code=newCode(_jr,NULL,newOperand(_reg,$RA),NULL);
			retCode=catCode(retCode,Code);
			break;
		case _DEC:
			alloc(list->stmt->arg1,list->stmt->arg2->val);
			free(Code);
			retCode=NULL;
			break;
		case _ARG:
			//Do nothing, deal with it in CALL
			free(Code);
			retCode=NULL;
			break;
		case _CALL: 
			Code->instr=_jal;
			Code->src1=NEWOPERAND;
			generateOperand(Code->src1,list->stmt->arg1,1);
			Code=NEWCODE;
			Code->instr=_move;
			Code->src1=newOperand(_reg,$V0);
			Code->dest=NEWOPERAND;
			retCode=catCode(retCode,Code);
			Code=retCode;
			retCode=callStack(list,retCode);
			retCode=catCode(retCode,generateOperand(Code->next->dest,list->stmt->target,0));
			break;
		case _PARA: 
			if(loadCount<4){
				Code->instr=_move;
				Code->dest=NEWOPERAND;
				Code->src1=newOperand(_reg,$A0+loadCount);
				retCode=catCode(generateOperand(Code->dest,list->stmt->target,1),retCode);
			}
			else{
				Code->instr=_lw;
				Code->dest=NEWOPERAND;
				Code->src1=newRefeOperand(-4*(loadCount-3),$FP);
				retCode=catCode(generateOperand(Code->dest,list->stmt->target,1),retCode);
			}
			loadCount++;
			break;
		case _READ:
			free(Code);
			retCode=NULL;
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
	//char *s=printInstr(retCode);
	//printf("%s\n",s);
	//free(s);
	return retCode;
}

funcSeg generateFunc(IRStmtList **head){
	funcSeg func=NEWFUNCSEG;
	func->instrHead=NULL;
	func->next=NULL;
	func->funcName=(*head)->stmt->arg1->name;
	funcActiveAnalyze(*head);
	graphColoring(*head);
	fpOffSet=0;
	loadCount=0;
	func->instrHead=funcStart();
	while((*head)->next!=0&&(*head)->next->stmt->type!=_FUNC){
		(*head)=(*head)->next;
		func->instrHead=catCode(func->instrHead,generateCode(head));
	}
	(*head)=(*head)->next;
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
	while((*list)!=NULL&&(*list)->next!=NULL){
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
	if(Code==NULL){
		return NULL;
	}
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
			sprintf(instr,"div %s, %s",src1,src2);
			free(src1);
			free(src2);
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
			sprintf(instr,"sw %s, %s",src1,dest);
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
		case _li:
			src1=printOperand(Code->src1);
			dest=printOperand(Code->dest);
			sprintf(instr,"li %s, %s",dest,src1);
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
		case _jr:
			src1=printOperand(Code->src1);
			sprintf(instr,"jr %s",src1);
			free(src1);
			break;
		case _jal:
			src1=printOperand(Code->src1);
			sprintf(instr,"jal %s",src1);
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

void printMachineCode(FILE *f,machineCode MC){
	dataItem item=MC->data;
	funcSeg func=MC->func;
	fprintf(f,".data\n");
	while(item!=NULL){
		char *typeStr=malloc(MAXTYPELEN*sizeof(char));
		char *dataStr=malloc(MAXDATALEN*sizeof(char));
		switch(item->type){
			case _asciiz:
				strcp(typeStr,".asciiz");
				strcp(dataStr,item->str);
				break;
			case _word:
				strcp(typeStr,".word");
				break;
			case _space:
				strcp(typeStr,".space");
				break;
		}
		fprintf(f,"%s: %s %s\n",item->name,typeStr,dataStr);
		free(typeStr);
		free(dataStr);
		item=item->next;
	}
	fprintf(f,".globl main\n");
	fprintf(f,".text\n");
	fprintf(f,"%s",builtInFunc);
	while(func!=NULL){
		fprintf(f,"%s:\n",func->funcName);
		code instr=func->instrHead;
		while(instr!=NULL){
			char *instrStr=printInstr(instr);
			if(instrStr!=NULL){
				if(instr->instr!=_lab){
					fprintf(f,"  ");
				}
				fprintf(f,"%s\n",instrStr);
				free(instrStr);
			}
			instr=instr->next;
		}
		fprintf(f,"\n");
		func=func->next;
	}
}
