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
	yylval=newNode(INT,atoi(yytext),0);
	return INT;
}

-?(({noneZeroDigit}{digit}*)|(0))\.{digit}+ {
	yylval=newFloatNode(FLOAT,atof(yytext),0);
	return FLOAT;
}

; {
	yylval=newNode(SEMI,yylineno,0);
	return SEMI;
}

, {
	yylval=newNode(COMMA,yylineno,0);
	return COMMA;
}

= {
	yylval=newNode(ASSIGNOP,yylineno,0);
	return ASSIGNOP;
}

>|<|(>=)|(<=)|(==)|(!=) {
	yylval=newNode(RELOP,yylineno,0);
	yylval->text=malloc(yyleng*sizeof(char));
	strcp(yylval->text,yytext);
	return RELOP;
}

\+ {
	yylval=newNode(PLUS,yylineno,0);
	return PLUS;
}

\- {
	yylval=newNode(MINUS,yylineno,0);
	return MINUS;
}

\* {
	yylval=newNode(STAR,yylineno,0);
	return STAR;
}

\/ {
	yylval=newNode(DIV,yylineno,0);
	return DIV;
}

&& {
	yylval=newNode(AND,yylineno,0);
	return AND;
}

\|\| {
	yylval=newNode(OR,yylineno,0);
	return OR;
}

\. {
	yylval=newNode(DOT,yylineno,0);
	return DOT;
}

! {
	yylval=newNode(NOT,yylineno,0);
	return NOT;
}

"int"|"float" {
	yylval=newNode(TYPE,yylineno,0);
	yylval->text=malloc(yyleng*sizeof(char));
	strcp(yylval->text,yytext);
	return TYPE;
}

\( {
	yylval=newNode(LP,yylineno,0);
	return LP;
}

\) {
	yylval=newNode(RP,yylineno,0);
	return RP;
}

\[ {
	yylval=newNode(LB,yylineno,0);
	return LB;
}

\] {
	yylval=newNode(RB,yylineno,0);
	return RB;
}

\{ {
	yylval=newNode(LC,yylineno,0);
	return LC;
}

\} {
	yylval=newNode(RC,yylineno,0);
	return RC;
}

"struct" {
	yylval=newNode(STRUCT,yylineno,0);
	return STRUCT;
}

"return" {
	yylval=newNode(RETURN,yylineno,0);
	return RETURN;
}

"if" {
	yylval=newNode(IF,yylineno,0);
	return IF;
}

"else" {
	yylval=newNode(ELSE,yylineno,0);
	return ELSE;
}

"while" {
	yylval=newNode(WHILE,yylineno,0);
	return WHILE;
}

{letter}({letter}|{digit}|_)* {
	yylval=newNode(ID,yylineno,0);
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


