#include "IR.h"
#include "machine.h"
#include "malloc.h"
#define NEWCODE (code)malloc(sizeof(struct _code))
#define NEWOPERAND (code)malloc(sizeof(struct _operand))

code generateOperand(operand OP,IRVar *var){
    operand *Operand=NEWOPERAND;
    switch(var->type){
        case VARIABLE:
        case TEMP:
            OP->type=_reg;
            OP->regNum=0;//TODO: implement getReg()
        break;
        case CONSTANT:

    }
}

code generateCode(IRStmtList **head){
    IRStmtList *list=*head;
    code Code=NEWCODE;
    switch(list->stmt->type){
	case _LABE: 
        break;
    case _FUNC: 
        break;
    case _ASSI: 
        break;
    case _ADD: 
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
}

