#ifndef __SYMBOLTABLE_H_
#define __SYMBOLTABLE_H_

#include"grammerTree.h"

#define TABLE_LEN 0x4000
#define PARA_LEN 0x80

typedef struct {
	char *name;
	int type;
	int arrayLen;//0=not array
} varibleItem;

typedef struct {
	char *name;
	int implemented;
	
	int length;
	varibleItem **parameters;
	int returnType;
} functionItem;

typedef struct{
	char *name;
	int length;
	varibleItem **field;
} structItem;

typedef struct structNode{
	structItem *item;
	struct structNode* next;
}structNode;


unsigned int hash_pjw(char *name);

void varibleInsert(treeNode *node,int type);

void functionInsert(treeNode *node);

int structInsert(treeNode *node);

varibleItem *varibleSearch(char *name);

functionItem *functionSearch(char *name);

int structSearch(char *name);

structItem *structGet(int type);
#endif
