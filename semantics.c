#include"semantics.h"
#include"symbolTable.h"
#include"malloc.h"
#include"stdio.h"

int specifierType;
extern int globalErrorFlag;

int checkExpError(treeNode *node){
	if(node->ExpType==-1){
		return 1;
	}
	else{
		return 0;
	}
}

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
			if(CHILD1(node)->genCount==1){
				if(varibleSearch(CHILD1(CHILD2(CHILD1(node)))->text)!=0){
					errorHandler(16,node->val);
				}
				if(structSearch(CHILD1(CHILD2(CHILD1(node)))->text)!=0){
					errorHandler(16,node->val);
				}
				return structInsert(CHILD1(node));
			}
			else{
				return structSearch(CHILD1(CHILD2(CHILD1(node)))->text);
			}
		}
	}
}

void DecListHandler(treeNode *node,int depth){
	if(node->type==VarDec&&node->parentNode->type!=VarDec){
		treeNode *name=node;
		while(name->genCount==2){
			name=CHILD1(name);
		}
		varibleItem *item=varibleSearch(CHILD1(name)->text);
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
							   if(node->genCount==3){
								   if(item->implemented==1){
									   errorHandler(4,node->val);
									   return;
								   }
							   }
							   else{
								   functionItem *titem=functionCreate(node);
								   if(titem->returnType!=item->returnType||titem->length!=item->length){
									   errorHandler(19,node->val);
									   free(titem);
									   return;
								   }
								   for(int i=0;i<item->length;i++){
									   if(item->parameters[i]->type!=titem->parameters[i]->type||
											   item->parameters[i]->arrayDim!=titem->parameters[i]->arrayDim){
										   errorHandler(19,node->val);
										   free(titem);
										   return;
									   }
								   }
								   free(titem);
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
					if(checkExpError(CHILD1(node))||checkExpError(CHILD3(node))){
						return;
					}
					switch(CHILD1(node)->genCount){
						case 14:
						case 15:
						case 16:
							break;
						default:
							errorHandler(6,node->val);
							return;
							break;
					}
					if(CHILD1(node)->ExpType!=CHILD3(node)->ExpType||
							CHILD1(node)->ExpDim!=0||CHILD3(node)->ExpDim!=0){
						errorHandler(5,node->val);
					}
					node->ExpType=CHILD1(node)->ExpType;
					break;
				case 2:
				case 3:
					if(checkExpError(CHILD1(node))||checkExpError(CHILD3(node))){
						return;
					}
					if(CHILD1(node)->ExpType!=_int||
							CHILD3(node)->ExpType!=_int||
							CHILD1(node)->ExpDim!=0||
							CHILD3(node)->ExpDim!=0){
						errorHandler(7,node->val);
						return;
					}
					node->ExpType=_int;
					break;
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					if(checkExpError(CHILD1(node))||checkExpError(CHILD3(node))){
						return;
					}
					if(!((CHILD1(node)->ExpType==_int||CHILD1(node)->ExpType==_float)&&
								CHILD1(node)->ExpType==CHILD3(node)->ExpType)||
							CHILD1(node)->ExpDim!=0||CHILD3(node)->ExpDim!=0){
						errorHandler(7,node->val);
						return;
					}
					node->ExpType=CHILD1(node)->ExpType;
					break;
				case 9:
					if(checkExpError(CHILD2(node))){
						return;
					}
					node->ExpType=CHILD2(node)->ExpType;
					break;
				case 10:
					if(checkExpError(CHILD2(node))){
						return;
					}
					if(!(CHILD2(node)->ExpType!=_int&&CHILD2(node)->ExpType!=_float)){
						errorHandler(7,node->val);
					}
					node->ExpType=CHILD2(node)->ExpType;
					break;
				case 11:
					if(checkExpError(CHILD2(node))){
						return;
					}
					if(!CHILD2(node)->ExpType!=_int){
						errorHandler(7,node->val);
					}
					node->ExpType=_int;
					break;
				case 12:{
							functionItem *item=functionSearch(CHILD1(node)->text);
							if(item==0){
								errorHandler(2,CHILD1(node)->val);
								return;
							}
							node->ExpType=item->returnType;
							break;
						}
				case 13:{
							functionItem *item=functionSearch(CHILD1(node)->text);
							if(item==0){
								errorHandler(2,CHILD1(node)->val);
								return;
							}
							treeNode *args=node;
							for(int i=0;i<item->length;i++){
								if(CHILD2(node)==0){
									errorHandler(9,args->val);
									return;
								}
								args=CHILD3(args);
								if(item->parameters[i]->type!=CHILD1(args)->ExpType){
									errorHandler(9,args->val);
									return;
								}
							}
							node->ExpType=item->returnType;
							break;
						}
				case 14:{
							if(checkExpError(CHILD1(node))||checkExpError(CHILD3(node))){
								return;
							}
							if(CHILD1(node)->ExpDim==0){
								errorHandler(10,node->val);
								return;
							}
							if(CHILD3(node)->ExpType!=_int){
								errorHandler(12,node->val);
								return;
							}
							node->ExpType=CHILD1(node)->ExpType;
							node->ExpDim=CHILD1(node)->ExpDim-1;
							break;
						}
				case 15:{
							if(checkExpError(CHILD1(node))){
								return;
							}
							if(CHILD1(node)->ExpType<2){
								errorHandler(13,node->val);
								return;
							}
							structItem *item=structGet(CHILD1(node)->ExpType);
							varibleItem *vitem=searchField(item,CHILD3(node)->text);
							if(vitem==0){
								errorHandler(14,node->val);
								return;
							}
							node->ExpType=vitem->type;
							node->ExpDim=vitem->arrayDim;
							break;
						}
				case 16:{//var
							varibleItem *item=varibleSearch(CHILD1(node)->text);
							if(item==0){
								functionItem *fitem=functionSearch(CHILD1(node)->text);
								if(fitem!=0){
									errorHandler(11,node->val);
									return;
								}
								errorHandler(1,node->val);
								return;
							}
							node->ExpType=item->type;
							node->ExpDim=item->arrayDim;
							break;
						}
				case 17:
						node->ExpType=_int;
						node->ExpDim=0;
						break;
				case 18:
						node->ExpType=_float;
						node->ExpDim=0;
						break;
			}
			break;
	}
	return;
}
