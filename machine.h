#ifndef __MACHINE_H_
#define __MACHINE_H_
#include "IR.h"

typedef enum {_label,_add,_addi,_sub,_mul,_div,_mflo,_move,_li,_lw,_sw,_j,_jal,_jr,_beq,_bne,_bgt,_blt,_bge,_ble} instrType;
typedef enum {_lab,_reg, _immi, _refe} operandType;
typedef enum {_asciiz,_word,_space} dataType;

struct _operand{
    operandType type;
    union{
            int regNum;
            int val;
        struct {
            int baseRegNum;
            int offset;
        };
        char *name;
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

struct _code{
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
struct _machineCode{
    dataItem data;
    funcSeg func;
};
typedef struct _machineCode* machineCode;

funcSeg generateFunc(IRStmtList **head);
code generateCode(IRStmtList **head);
machineCode generateProgram(IRStmtList *head);

void printMachineCode(machineCode MC);



#endif