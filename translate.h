#ifndef __TRANSLATE_H_
#define __TRANSLATE_H_
#include "IR.h"
#include "grammerTree.h"

IRStmtList *translate(treeNode *node);
IRStmtList *translateExp(IRVar *retVar,treeNode *node);
IRStmtList *translateExpStmt(treeNode *node);
IRStmtList *translateReturn(treeNode *node);
IRStmtList *translateIf(treeNode *node);
IRStmtList *translateWhile(treeNode *node);
IRStmtList *translateCond(treeNode *node,IRStmt *Lt,IRStmt *Lf);
#endif
