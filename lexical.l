%{
#include<stdio.h>
#include "grammerTree.h"
#include "syntax.tab.h"

#define YY_USER_ACTION
//	yylloc.first_line=yylloc.last_line=yylineno;
extern int globalErrorFlag;
extern int lastErrorPos;
%}

digit [0-9]
noneZeroDigit [1-9]
letter [a-zA-Z]

%option yylineno

%%
(-?{noneZeroDigit}{digit}*)|0 {
	yylval=newNode(INT,atoi(yytext));
	return INT;
}

-?(({noneZeroDigit}{digit}*)|(0))\.{digit}+ {
	yylval=newFloatNode(FLOAT,atof(yytext));
	return FLOAT;
}

; {
	yylval=newNode(SEMI,yylineno);
	return SEMI;
}

, {
	yylval=newNode(COMMA,yylineno);
	return COMMA;
}

= {
	yylval=newNode(ASSIGNOP,yylineno);
	return ASSIGNOP;
}

>|<|(>=)|(<=)|(==)|(!=) {
	yylval=newNode(RELOP,yylineno);
	yylval->text=malloc(yyleng*sizeof(char));
	strcp(yylval->text,yytext);
	return RELOP;
}

\+ {
	yylval=newNode(PLUS,yylineno);
	return PLUS;
}

\- {
	yylval=newNode(MINUS,yylineno);
	return MINUS;
}

\* {
	yylval=newNode(STAR,yylineno);
	return STAR;
}

\/ {
	yylval=newNode(DIV,yylineno);
	return DIV;
}

&& {
	yylval=newNode(AND,yylineno);
	return AND;
}

\|\| {
	yylval=newNode(OR,yylineno);
	return OR;
}

\. {
	yylval=newNode(DOT,yylineno);
	return DOT;
}

! {
	yylval=newNode(NOT,yylineno);
	return NOT;
}

"int"|"float" {
	yylval=newNode(TYPE,yylineno);
	yylval->text=malloc(yyleng*sizeof(char));
	strcp(yylval->text,yytext);
	return TYPE;
}

\( {
	yylval=newNode(LP,yylineno);
	return LP;
}

\) {
	yylval=newNode(RP,yylineno);
	return RP;
}

\[ {
	yylval=newNode(LB,yylineno);
	return LB;
}

\] {
	yylval=newNode(RB,yylineno);
	return RB;
}

\{ {
	yylval=newNode(LC,yylineno);
	return LC;
}

\} {
	yylval=newNode(RC,yylineno);
	return RC;
}

"struct" {
	yylval=newNode(STRUCT,yylineno);
	return STRUCT;
}

"return" {
	yylval=newNode(RETURN,yylineno);
	return RETURN;
}

"if" {
	yylval=newNode(IF,yylineno);
	return IF;
}

"else" {
	yylval=newNode(ELSE,yylineno);
	return ELSE;
}

"while" {
	yylval=newNode(WHILE,yylineno);
	return WHILE;
}

{letter}({letter}|{digit}|_)* {
	yylval=newNode(ID,yylineno);
	yylval->text=malloc(yyleng*sizeof(char));
	strcp(yylval->text,yytext);
	return ID;
}

\n|" "|\t|\r

"//" {
	char c=input();
	while(c!='\n' && c!='\r') c=input();
}

"/*" {
	char c=input();
	int state=0;
	while(state!=1 || c!='/'){
		if(c=='*'){
			state=1;
		}	
		else{
			state=0;
		}
		c=input();
	} 
}

. {
	globalErrorFlag=1;
	if(lastErrorPos!=yylineno){
		printf("Error type A at Line %d:unrecongnized token %s\n",yylineno,yytext);
	}
	lastErrorPos=yylineno;
}

%%


