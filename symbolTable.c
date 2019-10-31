#include"symbolTable.h"
#include"syntax.tab.h"
#include<malloc.h>

varibleItem *varTable[TABLE_LEN];
functionItem *funcTable[TABLE_LEN];

varibleItem *paraList[PARA_LEN];
int paraCount;

void loadParameters(treeNode *node,int depth){
	if(node->type==INT||node->type==FLOAT){
		varibleItem *newItem=malloc(sizeof(varibleItem));
		newItem->type=node->type;
		paraList[paraCount]=newItem;
	}
	else if(node->type==ID){
		paraList[paraCount++]->name=node->text;
	}
}

unsigned int hash_pjw(char *name){
	unsigned int val =0,i;
	for(;*name;name++){
		val=(val<<2)+*name;
		if(i=val&(TABLE_LEN-1)){
			val=(val^(i>>12)&(TABLE_LEN-1));
		}
	}
	return val;
}

void varibleInsert(treeNode *node){
	unsigned int pos=hash_pjw(node->text);
	while(varTable[pos]!=0){
		pos++;
		pos%=TABLE_LEN;
	}

	varTable[pos]=malloc(sizeof(varibleItem));

	varTable[pos]->name=node->text;
	varTable[pos]->type=node->type;
	return;
}

void functionInsert(treeNode *node){
	unsigned int pos=hash_pjw(node->text);
	while(funcTable[pos]!=0){
		pos++;
		pos%=TABLE_LEN;
	}

	funcTable[pos]=malloc(sizeof(varibleItem));

	funcTable[pos]->name=node->text;
	if(CHILD3(node)->type==SEMI){
		funcTable[pos]->implemented=0;
	}
	else{
		funcTable[pos]->implemented=1;
	}

	paraCount=0;
	travelNode(CHILD3(CHILD2(node)),loadParameters,0);
	funcTable[pos]->parameters=malloc(paraCount*sizeof(varibleItem*));
	for(int i=0;i<paraCount;i++){
		funcTable[pos]->parameters[i]=paraList[i];
	}

	funcTable[pos]->returnType=CHILD1(CHILD1(node))->type;
	return;
}

varibleItem *varibleSearch(char *name){
	unsigned int pos=hash_pjw(name);
	while(varTable[pos]!=0){
		if(strcm(name,varTable[pos]->name)==0){
			return varTable[pos];
		}
		pos++;
	}
	return 0;
}

functionItem *functionSearch(char *name){
	unsigned int pos=hash_pjw(name);
	while(funcTable[pos]!=0){
		if(strcm(name,funcTable[pos]->name)==0){
			return funcTable[pos];
		}
		pos++;
	}
	return 0;
}
