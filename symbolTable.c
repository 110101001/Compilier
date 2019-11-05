#include"symbolTable.h"
#include"semantics.h"
#include"syntax.tab.h"
#include<malloc.h>

varibleItem *varTable[TABLE_LEN];
functionItem *funcTable[TABLE_LEN];
structNode *structHead;

varibleItem *paraList[PARA_LEN];
int paraCount;


void loadParameters(treeNode *node,int depth){
	static int type;
	if(node->type==Specifier){
		type=specifierRead(node);
	}
	else if(node->type==VarDec){
		int arrayDim=0;
		treeNode *dec=node;
		while(CHILD2(dec)!=0){
			dec=CHILD1(dec);
			arrayDim++;
		}
		paraList[paraCount]=malloc(sizeof(varibleItem));
		paraList[paraCount]->type=type;
		paraList[paraCount]->name=CHILD1(dec)->text;
		paraList[paraCount]->arrayDim=arrayDim;
		if(arrayDim!=0){
			paraList[paraCount]->arrayLen=malloc(arrayDim*sizeof(int));
			for(int i=0;i<arrayDim;i++){
				dec=dec->parentNode;
				paraList[paraCount]->arrayLen[i]=CHILD3(dec)->val;
			}
		}
		paraCount++;
	}
}

varibleItem *searchField(structItem *item,char *name){
	for(int i=0;i<item->length;i++){
		if(strcm(item->field[i]->name,name)==0){
			return item->field[i];
		}
	}
	return 0;
}

unsigned int hash_pjw(char *name){
	unsigned int val =0,i;
	for(;*name;name++){
		val=(val<<2)+*name;
		if(i=val&(TABLE_LEN-1)){
			val=(val^(i>>12))&(TABLE_LEN-1);
		}
	}
	return val;
}

int structInsert(treeNode* node){
	int type=2;
	structNode *p=structHead;
	if(structHead==0){
		structHead=malloc(sizeof(structNode));
		p=structHead;
	}
	else{
		while(p->next!=0){
			p=p->next;
			type++;
		}
		p->next=malloc(sizeof(structNode));
		p=p->next;
	}

	p->next=0;
	p->item=malloc(sizeof(structItem));
	p->item->name=CHILD1(CHILD2(node))->text;

	paraCount=0;
	travelNode(CHILD4(node),loadParameters,0);
	p->item->length=paraCount;

	p->item->field=malloc(paraCount*sizeof(varibleItem*));
	for(int i=0;i<paraCount;i++){
		p->item->field[i]=paraList[i];
	}
	return type;
}

void varibleInsert(treeNode *node,int type){

	int arrayDim=0;
	treeNode *dec=node;
	while(CHILD2(dec)!=0){
		dec=CHILD1(dec);
		arrayDim++;
	}

	unsigned int pos=hash_pjw(CHILD1(dec)->text);
	while(varTable[pos]!=0){
		pos++;
		pos%=TABLE_LEN;
	}

	varTable[pos]=malloc(sizeof(varibleItem));

	varTable[pos]->name=CHILD1(dec)->text;
	varTable[pos]->type=type;

	varTable[pos]->arrayDim=arrayDim;
	if(arrayDim!=0){
		varTable[pos]->arrayLen=malloc(arrayDim*sizeof(int));
		for(int i=0;i<arrayDim;i++){
			dec=dec->parentNode;
			varTable[pos]->arrayLen[i]=CHILD3(dec)->val;
		}
	}
	return;
}

void functionInsert(treeNode *node){
	unsigned int pos=hash_pjw(CHILD1(CHILD2(node))->text);
	while(funcTable[pos]!=0){
		pos++;
		pos%=TABLE_LEN;
	}

	funcTable[pos]=malloc(sizeof(functionItem));

	funcTable[pos]->name=CHILD1(CHILD2(node))->text;
	if(node->genCount==4){
		funcTable[pos]->implemented=0;
	}
	else{
		funcTable[pos]->implemented=1;
	}

	paraCount=0;
	if(CHILD3(CHILD2(node))->type==VarList){
		travelNode(CHILD3(CHILD2(node)),loadParameters,0);
		funcTable[pos]->parameters=malloc(paraCount*sizeof(varibleItem*));
		for(int i=0;i<paraCount;i++){
			funcTable[pos]->parameters[i]=paraList[i];
		}
	}
	else{
		funcTable[pos]->parameters=0;
	}
	funcTable[pos]->length=paraCount;
	funcTable[pos]->returnType=specifierRead((CHILD1(node)));
	return;
}

functionItem *functionCreate(treeNode *node){
	functionItem *item;
	item=malloc(sizeof(functionItem));

	item->name=CHILD1(CHILD2(node))->text;
	if(node->genCount==4){
		item->implemented=0;
	}
	else{
		item->implemented=1;
	}

	paraCount=0;
	if(CHILD3(CHILD2(node))->type==VarList){
		travelNode(CHILD3(CHILD2(node)),loadParameters,0);
		item->parameters=malloc(paraCount*sizeof(varibleItem*));
		for(int i=0;i<paraCount;i++){
			item->parameters[i]=paraList[i];
		}
	}
	else{
		item->parameters=0;
	}
	item->length=paraCount;
	item->returnType=specifierRead((CHILD1(node)));
	return item;
}
int structSearch(char *name){
	int type=2;
	structNode *p=structHead;
	while(p!=0){
		if(strcm(p->item->name,name)==0){
			return type;
		}
		type++;
		p=p->next;
	}
	return 0;
}

structItem *structGet(int type){
	structNode *p=structHead;
	for(int i=0;i<type-2;i++){
		p=p->next;
	}
	return p->item;
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
