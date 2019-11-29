%{
#include "grammerTree.h"
#include "lex.yy.c"
#include<stdio.h>

int globalErrorFlag;
int lastErrorPos=-1;

int yydebug=0;

void yyerror(char *msg){
	globalErrorFlag=1;
	if(lastErrorPos!=yylineno){
		printf("Error type B at Line %d:%s\n",yylineno,msg);
	}
	lastErrorPos=yylineno;
}


extern treeNode* root;
%}

%locations

%token ID INT FLOAT
%token RELOP ASSIGNOP PLUS MINUS STAR DIV AND OR NOT
%token LP RP LB RB LC RC
%token IF ELSE WHILE RETURN TYPE STRUCT DOT SEMI COMMA

%left PLUS MINUS
%left STAR DIV

%%
Program : ExtDefList {
		treeNode* node=newNode(Program,$1->val,1);
		insertNode(node,$1);
		$$=node;
		root=node;
		}

ExtDefList : ExtDef ExtDefList{
		treeNode* node=newNode(ExtDefList,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		$$=node;
		}
		   | /*EMPTY*/{
		   $$=0;
		   }

ExtDef : Specifier ExtDecList SEMI {
		treeNode* node=newNode(ExtDef,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	   | Specifier SEMI {
		treeNode* node=newNode(ExtDef,$1->val,2);
		insertNode(node,$1);
		insertNode(node,$2);
		$$=node;
		}
	   | Specifier FunDec CompSt{
		treeNode* node=newNode(ExtDef,$1->val,3);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
		| Specifier FunDec SEMI{
		treeNode* node=newNode(ExtDef,$1->val,4);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;	
		}

ExtDecList : VarDec{
		treeNode* node=newNode(ExtDecList,$1->val,1);
		insertNode(node,$1);
		$$=node;
		}
		| VarDec COMMA ExtDecList{
		treeNode* node=newNode(ExtDecList,$1->val,2);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}

Specifier : TYPE{
		treeNode* node=newNode(Specifier,$1->val,1);
		insertNode(node,$1);
		$$=node;
		}
		| StructSpecifier{
		treeNode* node=newNode(Specifier,$1->val,2);
		insertNode(node,$1);
		$$=node;
		}

StructSpecifier : STRUCT OptTag LC DefList RC{
		treeNode* node=newNode(StructSpecifier,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		insertNode(node,$4);
		insertNode(node,$5);
		$$=node;
		}
		| STRUCT Tag{
		treeNode* node=newNode(StructSpecifier,$1->val,2);
		insertNode(node,$1);
		insertNode(node,$2);
		$$=node;
		}

OptTag : ID{
		treeNode* node=newNode(OptTag,$1->val,1);
		insertNode(node,$1);
		$$=node;
		}
	   | /*EMPTY*/{
		treeNode* node=newNode(OptTag,0,2);
	   	$$=node;
		}

Tag : ID{
		treeNode* node=newNode(Tag,$1->val,1);
		insertNode(node,$1);
		$$=node;
		}

VarDec : ID{
		treeNode* node=newNode(VarDec,$1->val,1);
		insertNode(node,$1);
		$$=node;
		}
	   | VarDec LB INT RB{
		treeNode* node=newNode(VarDec,$1->val,2);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		insertNode(node,$4);
		$$=node;
		}

FunDec : ID LP VarList RP {
		treeNode* node=newNode(FunDec,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		insertNode(node,$4);
		$$=node;
		}
	   | ID LP RP{
		treeNode* node=newNode(FunDec,$1->val,2);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		} 

VarList : ParamDec COMMA VarList{
		treeNode* node=newNode(VarList,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
		| ParamDec{
		treeNode* node=newNode(VarList,$1->val,2);
		insertNode(node,$1);
		$$=node;
		}

ParamDec : Specifier VarDec{
		treeNode* node=newNode(ParamDec,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		$$=node;
		}

CompSt : LC DefList StmtList RC{
		treeNode* node=newNode(CompSt,$1->val,2);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		insertNode(node,$4);
		$$=node;
		}
	   | error RC

StmtList : Stmt StmtList{
		treeNode* node=newNode(StmtList,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		$$=node;
		}
		| /*EMPTY*/{
			$$=0;			
}

Stmt : Exp SEMI{
		treeNode* node=newNode(Stmt,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		$$=node;
		}
	 | CompSt{
		treeNode* node=newNode(Stmt,$1->val,2);
		insertNode(node,$1);
		$$=node;
		}
	 | RETURN Exp SEMI {
		treeNode* node=newNode(Stmt,$1->val,3);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	 | IF LP Exp RP Stmt{
		treeNode* node=newNode(Stmt,$1->val,4);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		insertNode(node,$4);
		insertNode(node,$5);
		$$=node;
		}
	 | IF LP Exp RP Stmt ELSE Stmt{
		treeNode* node=newNode(Stmt,$1->val,5);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		insertNode(node,$4);
		insertNode(node,$5);
		insertNode(node,$6);
		insertNode(node,$7);
		$$=node;
		}
	 | WHILE LP Exp RP Stmt{
		treeNode* node=newNode(Stmt,$1->val,6);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		insertNode(node,$4);
		insertNode(node,$5);
		$$=node;
		}
	 | error SEMI

DefList : Def DefList{
		treeNode* node=newNode(DefList,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		$$=node;
		}
		| /*EMPTY*/{
			$$=0;
		}

Def : Specifier DecList SEMI{
		treeNode* node=newNode(Def,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}

DecList : Dec{
		treeNode* node=newNode(DecList,$1->val,1);
		insertNode(node,$1);
		$$=node;
		}
		| Dec COMMA DecList{
		treeNode* node=newNode(DecList,$1->val,2);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
		| error COMMA DecList

Dec : VarDec{
		treeNode* node=newNode(Dec,$1->val,1);
		insertNode(node,$1);
		$$=node;
		}
	| VarDec ASSIGNOP Exp{
		treeNode* node=newNode(Dec,$1->val,2);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}

Exp : Exp ASSIGNOP Exp{
		treeNode* node=newNode(Exp,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| Exp AND Exp{
		treeNode* node=newNode(Exp,$1->val,2);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| Exp OR Exp{
		treeNode* node=newNode(Exp,$1->val,3);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| Exp RELOP Exp{
		treeNode* node=newNode(Exp,$1->val,4);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| Exp STAR Exp{
		treeNode* node=newNode(Exp,$1->val,7);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| Exp DIV Exp{
		treeNode* node=newNode(Exp,$1->val,8);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| Exp PLUS Exp{
		treeNode* node=newNode(Exp,$1->val,5);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| Exp MINUS Exp{
		treeNode* node=newNode(Exp,$1->val,6);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| LP Exp RP{
		treeNode* node=newNode(Exp,$1->val,9);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| MINUS Exp{
		treeNode* node=newNode(Exp,$1->val,10);
		insertNode(node,$1);
		insertNode(node,$2);
		$$=node;
		}
	| NOT Exp{
		treeNode* node=newNode(Exp,$1->val,11);
		insertNode(node,$1);
		insertNode(node,$2);
		$$=node;
		}
	| ID LP Args RP{
		treeNode* node=newNode(Exp,$1->val,12);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		insertNode(node,$4);
		$$=node;
		}
	| ID LP RP{
		treeNode* node=newNode(Exp,$1->val,13);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| Exp LB Exp RB{
		treeNode* node=newNode(Exp,$1->val,14);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		insertNode(node,$4);
		$$=node;
		}
	| Exp DOT ID{
		treeNode* node=newNode(Exp,$1->val,15);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	| ID{
		treeNode* node=newNode(Exp,$1->val,16);
		insertNode(node,$1);
		$$=node;
		}
	| INT{
		treeNode* node=newNode(Exp,yylineno,17);
		insertNode(node,$1);
		$$=node;
		}
	| FLOAT{
		treeNode* node=newNode(Exp,yylineno,18);
		insertNode(node,$1);
		$$=node;
		}

Args : Exp COMMA Args{
		treeNode* node=newNode(Args,$1->val,1);
		insertNode(node,$1);
		insertNode(node,$2);
		insertNode(node,$3);
		$$=node;
		}
	 | Exp{
		treeNode* node=newNode(Args,$1->val,2);
		insertNode(node,$1);
		$$=node;
		}

%%
