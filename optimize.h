#ifndef _OPTIMIZE_H_
#define _OPTIMIZE_H_
#include "IR.h"

int findVar(IRVar **list,int len,IRVar *var);
IRStmtList *IROptimize(IRStmtList *head);

#endif
