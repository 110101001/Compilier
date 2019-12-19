#ifndef __machine_H_
#define __machine_H_

typedef enum {} instrType;
typedef enum {_reg, _immi, _refe} operandType;

struct operand{
    operandType type;
    union{
        {
            int regNum;
        };
        {
            int num;
        };
        {
            int regNum;
            int offset;
        };
    };
};
typedef struct operand* operand;

typedef struct code{
    instrType instr;
    operand src1;
    operand src2;
    operand dest;
    struct code* next;
};
typedef struct code* code;

struct funcSeg{
    char *funcName;
    code instrHead;
    struct funcSeg* next;
};
typedef funcSeg* funcSeg;

struct textSeg{
    funcSeg func;
};
typedef struct textSeg* textSeg;

struct dataSeg{
    
};
typedef struct dataSeg* dataSeg;

typedef struct machineCode{

}machineCode;


#endif