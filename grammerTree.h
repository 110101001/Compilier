#ifndef __GRAMMERTREE_H_
#define __GRAMMERTREE_H_

#define CHILD1(node) ((node)->children)
#define CHILD2(node) ((node)->children->next)
#define CHILD3(node) ((node)->children->next->next)
#define CHILD4(node) ((node)->children->next->next->next)

enum symbols{Program, ExtDefList, ExtDef,ExtDecList,Specifier,StructSpecifier,OptTag,Tag,VarDec,FunDec,VarList,ParamDec,CompSt,StmtList,Stmt,DefList,Def,DecList,Dec,Exp,Args};

enum{_int,_float};

typedef union inhAttr{
}inhAttr;

typedef union sys{
	struct{
		int ExpType;
		int ExpDim;
	};
}sysAttr;

typedef struct treeNode{
	int type;
	char visited;
	union{
		int val;
		float fval;
	};
	char *text;
	int genCount;
	inhAttr *inh;
	sysAttr *sys;
	struct treeNode *parentNode;
	struct treeNode* children;
	struct treeNode* next;
} treeNode;

#define YYSTYPE treeNode*

void strcp(char *a,char *b);
int strcm(char *a,char *b);

treeNode* newNode(int type,int val,int genCount);
treeNode* newFloatNode(int type,float val,int genCount);
void insertNode(treeNode* pnode,treeNode* cnode);
void deleteNode(treeNode* node);
void travelNode(treeNode* root,void (*action)(treeNode*,int),int depth);
void travelNodeRev(treeNode* root,void (*action)(treeNode*,int),int depth);

#endif
