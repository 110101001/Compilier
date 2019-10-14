#ifndef __GRAMMERTREE_H_
#define __GRAMMERTREE_H_

typedef struct treeNode{
	int type;
	union{
		int val;
		float fval;
	};
	char *text;
	struct treeNode *parentNode;
	struct treeNode* children;
	struct treeNode* next;
} treeNode;

#define YYSTYPE treeNode*

void strcp(char *a,char *b);

treeNode* newNode(int type,int val);
treeNode* newFloatNode(int type,float val);
void insertNode(treeNode* pnode,treeNode* cnode);
void deleteNode(treeNode* node);
void travelNode(treeNode* root,void (*action)(treeNode*,int),int depth);

#endif
