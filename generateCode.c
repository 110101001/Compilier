#include "IR.h"
#include "machine.h"
#include "malloc.h"
#define NEWCODE (code)malloc(sizeof(struct _code))
#define NEWOPERAND (code)malloc(sizeof(struct _operand))

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
            op->name=var->name;
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
    return NULL:
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
        break;
    case _MULT: 
        break;
    case _DIVI: 
        break;
    case _ADDR: 
        break;
    case _REFE: 
        break;
    case _WMEM:
        break;
    case _GOTO: 
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
    }
    return Code;
}

