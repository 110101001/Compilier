#ifndef __SYMBOLTABLE_H_
#define __SYMBOLTABLE_H_

#include"grammerTree.h"

#define TABLE_LEN 0x4000
#define PARA_LEN 0x80

typedef struct {
	char *name;
	int type;
} varibleItem;

typedef struct {
	char *name;
	int implemented;
	
	int length;
	varibleItem **parameters;
	int returnType;
} functionItem;

unsigned int hash_pjw(char *name);

void varibleInsert(treeNode *node);

void functionInsert(treeNode *node);

varibleItem *varibleSearch(char *name);

functionItem *functionSearch(char *name);

#endif
