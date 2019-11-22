#include "grammerTree.h"
#include "malloc.h"

treeNode* root;
char visitedFlag;

void strcp(char *a,char *b){
	while(*b!=0){
		*a=*b;
		a++;
		b++;
	}
}

int strcm(char *a,char *b){
	while(*b!=0){
		if(*a!=*b){
			return *a>*b?1:-1;
		}
		a++;
		b++;
	}
	if(*a!=0){
		return -1;
	}
	return 0;
}

treeNode* newNode(int type,int val,int genCount){
	treeNode* node;
	node=malloc(sizeof(treeNode));
	node->type=type;
	node->val=val;
	node->children=0;
	node->next=0;
	node->parentNode=0;
	node->genCount=genCount;
	node->sys=malloc(sizeof(sysAttr));
	node->inh=malloc(sizeof(inhAttr));
	switch(type){
		case 19:
			node->sys->ExpType=-1;
			node->sys->ExpDim=0;
			break;	
	}
	return node; 
}

treeNode* newFloatNode(int type,float val,int genCount){
	treeNode* node;
	node=malloc(sizeof(treeNode));
	node->type=type;
	node->fval=val;
	node->children=0;
	node->next=0;
	node->parentNode=0;
	node->genCount=genCount;
	node->sys=malloc(sizeof(sysAttr));
	node->inh=malloc(sizeof(inhAttr));
	switch(type){
		case 19:
			node->sys->ExpType=-1;
			node->sys->ExpDim=0;
			break;	
	}
	return node; 
}

void insertNode(treeNode* pnode,treeNode* cnode){
	treeNode *p=pnode->children;
	if(p==0){
		pnode->children=cnode;
	}
	else{
		while(p->next!=0){
			p=p->next;
		}
		p->next=cnode;
	}
	if(cnode!=0){
		cnode->parentNode=pnode;
	}
	return;
}
//void deleteNode(treeNode* node);
void travelNode(treeNode* root,void (*action)(treeNode*,int),int depth){
	if(root->visited==visitedFlag){
	treeNode *p=root->children;
	while(p!=0){
		travelNode(p,action,depth+1);
		p=p->next;
	}
	action(root,depth);
	root->visited=!visitedFlag;
	}
	return;
}

void travelNodeRev(treeNode* root,void (*action)(treeNode*,int),int depth){
	if(root->visited==visitedFlag){
	treeNode *p=root->children;
	action(root,depth);
	while(p!=0){
		travelNode(p,action,depth+1);
		p=p->next;
	}
	root->visited=!visitedFlag;
	}
	return;
}
