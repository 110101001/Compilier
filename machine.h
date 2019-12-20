#ifndef __machine_H_
#define __machine_H_
#include "IR.h"

typedef enum {_label,_add,_addi,_sub,_mul,_div,_mflo,_move,_li,_lw,_sw,_j,_jal,_jr,_beq,_bne,_bgt,_blt,_bge,_bie} instrType;
typedef enum {_label,_reg, _immi, _refe} operandType;
typedef enum {_ascii,_asciiz,_word,_space} dataType;

struct _operand{
    operandType type;
    union{
        struct {
            int regNum;
        };
        struct {
            int num;
        };
        struct {
            int regNum;
            int offset;
        };
    };
};
typedef struct _operand* operand;

struct _dataItem{
    dataType type;
    char *name;
    int len;
    union{
            char *str;
            int *val;
    };
    struct _dataItem *next;
};
typedef struct _dataItem* dataItem;

typedef struct _code{
    instrType instr;
    operand src1;
    operand src2;
    operand dest;
    struct _code* next;
};
typedef struct _code* code;

struct _funcSeg{
    char *funcName;
    code instrHead;
    struct _funcSeg* next;
};
typedef struct _funcSeg* funcSeg;

/*struct textSeg{
    funcSeg func;
};
typedef struct textSeg* textSeg;

struct dataSeg{
    
};
typedef struct dataSeg* dataSeg;
*/
typedef struct machineCode{
    dataItem data;
    funcSeg func;
}machineCode;

struct _dataItem promptStr={
    _asciiz,"_prompt",18,"Enter an integer:",&retStr
};

struct _dataItem retStr={
    _asciiz,"_ret",2,"\n",0
};

const dataItem standardDataSeg=&promptStr;

funcSeg generateFunc(IRStmtList **head);
code generateCode(IRStmtList **head);




#endif