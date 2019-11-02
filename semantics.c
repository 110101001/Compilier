#include"semantics.h"
#include"symbolTable.h"
#include"stdio.h"

int specifierType;
extern int globalErrorFlag;
void errorHandler(int type,int line){
	printf("Error type %d at Line %d :\n",type,line);
}

int specifierRead(treeNode *node){
	if(node->type==Specifier){
		if(CHILD1(node)->type==TYPE){
			if(strcm(CHILD1(node)->text,"int")==0){
				return _int;
			}
			else{
				return _float;
			}
		}
		else{
			if(varibleSearch(CHILD1(CHILD2(CHILD1(node)))->text)!=0){
				errorHandler(16,node->val);
			}
			if(structSearch(CHILD1(CHILD2(CHILD1(node)))->text)!=0){
				errorHandler(16,node->val);
			}
			return structInsert(CHILD1(node));
		}
	}
}

void DecListHandler(treeNode *node,int depth){
	if(node->type==VarDec){
		varibleItem *item=varibleSearch(CHILD1(node)->text);
		if(item!=0){
			errorHandler(3,node->val);
			return;
		}
		varibleInsert(node,specifierType);
	}	
}

void loadSymbol(treeNode *node,int depth){
	switch((enum symbols)node->type){
		case ExtDef:
			switch(node->genCount){
				case 1://global var dec
					specifierType=specifierRead(CHILD1(node));
					travelNode(CHILD2(node),DecListHandler,0);
					break;
				case 2://structure def or nothing meaningful(int;||float;)
					specifierRead(CHILD1(node));
					break;
				case 3:
				case 4:{//function declear||define
						   functionItem* item=functionSearch(CHILD1(CHILD2(node))->text);
						   if(item!=0){
							   if(CHILD3(node)->type!=SEMI){
								   if(item->implemented==1){
									   errorHandler(4,node->val);
									   return;
								   }
							   }
							   item->implemented=1;
						   }
						   else{
							   functionInsert(node);
						   }
						   break;
					   }
			}
			break;
		case Def:
			specifierType=specifierRead(CHILD1(node));
			travelNode(CHILD2(node),DecListHandler,0);	
			break;
		case  Exp:
			switch(node->genCount){
				case 1:
					switch(CHILD1(node)->genCount){
						case 14:
						case 15:
						case 16:
							break;
						default:
							errorHandler(6,node->val);
							break;
					}
					break;
				case 12://func
				case 13:{
							functionItem *item=functionSearch(CHILD1(node)->text);
							if(item==0){
								errorHandler(2,CHILD1(node)->val);
								return;
							}

							break;
						}
				case 16:{//var
							varibleItem *item=varibleSearch(CHILD1(node)->text);
							if(item==0){
								errorHandler(1,node->val);
								return;
							}
							break;
						}
			}
			break;
	}
	return;
}

