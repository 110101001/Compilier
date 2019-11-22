#ifndef __TRANSLATE_H_
#define __TRANSLATE_H_
#include "IR.h"
#include "grammerTree.h"

IRStmtList *translate(treeNode *node);
IRStmtList *translateExp(treeNode *node);
IRStmtList *translateStmt(treeNode *node);
#endif
