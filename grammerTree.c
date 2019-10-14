#include "grammerTree.h"
#include "malloc.h"

treeNode* root;

void strcp(char *a,char *b){
	while(*b!=0){
		*a=*b;
		a++;
		b++;
	}
}

treeNode* newNode(int type,int val){
	treeNode* node;
	node=malloc(sizeof(treeNode));
	node->type=type;
	node->val=val;
	node->children=0;
	node->next=0;
	node->parentNode=0;
	return node; 
}

treeNode* newFloatNode(int type,float val){
	treeNode* node;
	node=malloc(sizeof(treeNode));
	node->type=type;
	node->fval=val;
	node->children=0;
	node->next=0;
	node->parentNode=0;
	return node; 
}

void insertNode(treeNode* pnode,treeNode* cnode){
	treeNode *p=pnode->children;
	if(p==0){
		pnode->children=cnode;
		return;
	}
	while(p->next!=0){
		p=p->next;
	}
	p->next=cnode;
	if(cnode!=0){
		cnode->parentNode=pnode;
	}
	return;
}
//void deleteNode(treeNode* node);
void travelNode(treeNode* root,void (*action)(treeNode*,int),int depth){
	action(root,depth);
	treeNode *p=root->children;
	while(p!=0){
		travelNode(p,action,depth+1);
		p=p->next;
	}
	return;
}
