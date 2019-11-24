#ifndef __TRANSLATE_H_
#define __TRANSLATE_H_
#include "IR.h"
#include "grammerTree.h"

void combineCode(treeNode *node);
IRStmtList *translate(treeNode *node);
IRStmtList *translateStmt(treeNode *node);
IRStmtList *translateDec(treeNode *node);
IRStmtList *translateDefList(treeNode *node);
IRStmtList *translateExtDef(treeNode *node);
IRStmtList *translateFunction(treeNode *node);
IRStmtList *translateCall(IRVar *retVar,treeNode *node);
IRStmtList *translateCompSt(treeNode *node);
IRStmtList *translateStmtList(treeNode *node);
IRStmtList *translateExp(IRVar *retVar,treeNode *node);
IRStmtList *translateExpStmt(treeNode *node);
IRStmtList *translateReturn(treeNode *node);
IRStmtList *translateIf(treeNode *node);
IRStmtList *translateWhile(treeNode *node);
IRStmtList *translateCond(treeNode *node,IRVar *Lt,IRVar *Lf);
IRStmtList *translateArrayDec(treeNode *node);
IRStmtList *translateArray(IRVar *retVar,treeNode *node);
#endif
