#include<stdio.h>
#include "grammerTree.h"
#include "syntax.tab.h"
#include "semantics.h"

extern treeNode* root;
extern int globalErrorFlag;
extern char visitedFlag;
void printInfo(treeNode *node,int depth);

int main(int argc,char **argv){
	globalErrorFlag=0;
	//load file
	if(argc>=2){
		FILE *f;
		f=fopen(argv[1],"r");
		//parse
		yyrestart(f);
	}
	yyparse();

	//generate symboltable
	if(globalErrorFlag==0){
		travelNode(root,loadSymbol,0);
		visitedFlag=!visitedFlag;
	}
	return 0;
}

void NonTerminalOutput(treeNode *node){
	if(node->type<=20){
		switch(node->type){
			case 0:
					printf("Program");
				break;
			case 1:
					printf("ExtDefList");
				break;
			case 2:
					printf("ExtDef");
				break;
			case 3:
					printf("ExtDecList");
				break;
			case 4:
					printf("Specifier");
				break;
			case 5:
					printf("StructSpecifier");
				break;
			case 6:
					printf("OptTag");
				break;
			case 7:
					printf("Tag");
				break;
			case 8:
					printf("VarDec");
				break;
			case 9:
					printf("FunDec");
				break;
			case 10:
					printf("VarList");
				break;
			case 11:
					printf("ParamDec");
				break;
			case 12:
					printf("CompSt");
				break;
			case 13:
					printf("StmtList");
				break;
			case 14:
					printf("Stmt");
				break;
			case 15:
					printf("DefList");
				break;
			case 16:
					printf("Def");
				break;
			case 17:
					printf("DecList");
				break;
			case 18:
					printf("Dec");
				break;
			case 19:
					printf("Exp");
				break;
			case 20:
					printf("Args");
				break;
		}
		printf(" (%d)",node->val);
	}
}

void SymbolOutput(treeNode *node){
	if(node->type>=261){
		switch(node->type){
			case  261: printf("RELOP ");  break;  
			case  262: printf("ASSIGNOP ");  break;  
			case  263: printf("PLUS ");  break;  
			case  264: printf("MINUS ");  break;  
			case  265: printf("STAR ");  break;  
			case  266: printf("DIV ");  break;  
			case  267: printf("AND ");  break;  
			case  268: printf("OR ");  break;  
			case  269: printf("NOT ");  break;  
			case  270: printf("LP ");  break;  
			case  271: printf("RP ");  break;  
			case  272: printf("LB ");  break;  
			case  273: printf("RB ");  break;  
			case  274: printf("LC ");  break;  
			case  275: printf("RC ");  break;  
			case  276: printf("IF ");  break;  
			case  277: printf("ELSE ");  break;  
			case  278: printf("WHILE ");  break;  
			case  279: printf("RETURN ");  break;  
			case  280: printf("TYPE: "); printf("%s",node->text); break;  
			case  281: printf("STRUCT ");  break;  
			case  282: printf("DOT ");  break;  
			case  283: printf("SEMI ");  break;  
			case  284: printf("COMMA ");  break;  
		}
	}
}

void varOutput(treeNode *node){
	switch(node->type){
		case 258:
			printf("ID: %s",node->text);
			break;
		case 259:
			printf("INT: %d",node->val);
			break;
		case 260:
			printf("FLOAT: %f",node->fval);
			break;
	}	
}

void printInfo(treeNode *node,int depth){
	for(int i=0;i<depth;i++){
		printf("  ");
	}
	NonTerminalOutput(node);
	SymbolOutput(node);
	varOutput(node);
	printf("\n");
}


