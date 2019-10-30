#ifndef __SYMBOLTABLE_H_
#define __SYMBOLTABLE_H_

#include"grammerTree.h"

typedef struct {
	char *name;
	int type;
} varibleItem;

typedef parameterList{
	varibleItem *list;
	int length;
} parameterList;

typedef struct {
	char *name;
	int implemented;

	parameterList *parameters;
	varibleItem *returnVal;
} functionItem;

undesigned int hash_pjw(char *name);

void varibleInsert(treeNode *node);

void functionInsert(treeNode *node);


#endif
