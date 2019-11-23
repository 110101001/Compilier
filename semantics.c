#include"semantics.h"
#include"symbolTable.h"
#include"malloc.h"
#include"stdio.h"

int specifierType;
extern int globalErrorFlag;
extern int unimplementedFunctions;
extern int yylineno;
const char ErrorMessage[19][100]={
	"Using Undefined Varible",
	"Using Undefined Function",
	"Repeated Varible Defination",
	"Repeated Function Defination",
	"Unmatched Expression Type",
	"Right Value Appears on the Left Side of Assign Operator",
	"Illegal Expression Type of The operator",
	"Return Type is Unmatched",
	"Parameters are Unmatched",
	"Accessing Non Array Varible as a Array",
	"Calling Varible as a Function",
	"Expression in Array Access Operator is not an Integar",
	"Accessing Non Struct Varible as a Struct",
	"Accessing Unexisted Field",
	"Illegal Field",
	"Repeated Struct Name",
	"Using Undefined Struct",
	"Function Not Implemented",
	"Conflicted Function Declearation"
};
int checkExpError(treeNode *node){
	if(node->sys->ExpType==-1){
		return 1;
	}
	else{
		return 0;
	}
}

void errorHandler(int type,int line){
	printf("Error type %d at Line %d : %s \n",type,line,ErrorMessage[type-1]);
	globalErrorFlag=1;
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
				if(CHILD2(CHILD1(node))->genCount==2){
					return structInsert(CHILD1(node));
				}
				if(varibleSearch(CHILD1(CHILD2(CHILD1(node)))->text)!=0){
					errorHandler(16,node->val);
				}
				if(structSearch(CHILD1(CHILD2(CHILD1(node)))->text)!=0){
					errorHandler(16,node->val);
				}
				return structInsert(CHILD1(node));
			}
			else{
				int type = structSearch(CHILD1(CHILD2(CHILD1(node)))->text);
				if(type==0){
					errorHandler(17,node->val);
					return -1;
				}
				return type;
			}
		}
	}
}

void DecListHandler(treeNode *node){
	if(node->type==VarDec&&node->parentNode->type!=VarDec){
		treeNode *name=node;
		while(name->genCount==2){
			name=CHILD1(name);
		}
		varibleItem *item=varibleSearch(CHILD1(name)->text);
		if(item!=0){
			while(name->type!=CompSt&&name->type!=StructSpecifier){
				name=name->parentNode;
			}
			if(name->type==StructSpecifier&&item->parentName!=0){
				if(strcm(item->parentName,CHILD1(CHILD2(name))->text)==0){
					errorHandler(15,node->val);
					return;
				}
			}
			errorHandler(3,node->val);
			return;
		}
		if(node->parentNode->type==Dec&&node->next!=0){
			name=node;
			while(name->type!=CompSt&&name->type!=StructSpecifier){
				name=name->parentNode;
			}
			if(name->type==StructSpecifier){
				errorHandler(15,node->val);
			}
			if(node->next->next->sys->ExpType!=specifierType){
				errorHandler(5,node->val);
				return;
			}
		}
		varibleInsert(node,specifierType);
	}	
}

void loadSymbol(treeNode *node){
	switch((enum symbols)node->type){
		case ExtDef:
			switch(node->genCount){
				case 1://global var dec
					specifierType=specifierRead(CHILD1(node));
					travelNode(CHILD2(node),DecListHandler);
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
							   unimplementedFunctions--;
						   }
						   else{
							   functionInsert(node);
						   }
						   break;
					   }
			}
			break;
		case ParamDec:{
						  treeNode *func=node;
						  while(func->type!=FunDec){
							  func=func->parentNode;
						  }
						  if(functionSearch(CHILD1(func)->text)!=0){
							  break;
						  }
					  }
					  //don't break if the func is not loaded, act like def.
		case Def:
					  specifierType=specifierRead(CHILD1(node));
					  travelNode(CHILD2(node),DecListHandler);
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
							  if(CHILD1(node)->sys->ExpType!=CHILD3(node)->sys->ExpType||
									  CHILD1(node)->sys->ExpDim!=0||CHILD3(node)->sys->ExpDim!=0){
								  errorHandler(5,node->val);
							  }
							  node->sys->ExpType=CHILD1(node)->sys->ExpType;
							  break;
						  case 2:
						  case 3:
							  if(checkExpError(CHILD1(node))||checkExpError(CHILD3(node))){
								  return;
							  }
							  if(CHILD1(node)->sys->ExpType!=_int||
									  CHILD3(node)->sys->ExpType!=_int||
									  CHILD1(node)->sys->ExpDim!=0||
									  CHILD3(node)->sys->ExpDim!=0){
								  errorHandler(7,node->val);
								  return;
							  }
							  node->sys->ExpType=_int;
							  break;
						  case 4:
							  if(checkExpError(CHILD1(node))||checkExpError(CHILD3(node))){
								  return;
							  }
							  if(!((CHILD1(node)->sys->ExpType==_int||CHILD1(node)->sys->ExpType==_float)&&
										  CHILD1(node)->sys->ExpType==CHILD3(node)->sys->ExpType)||
									  CHILD1(node)->sys->ExpDim!=0||CHILD3(node)->sys->ExpDim!=0){
								  errorHandler(7,node->val);
								  return;
							  }
							  node->sys->ExpType=_int;
							  break;
						  case 5:
						  case 6:
						  case 7:
						  case 8:
							  if(checkExpError(CHILD1(node))||checkExpError(CHILD3(node))){
								  return;
							  }
							  if(!((CHILD1(node)->sys->ExpType==_int||CHILD1(node)->sys->ExpType==_float)&&
										  CHILD1(node)->sys->ExpType==CHILD3(node)->sys->ExpType)||
									  CHILD1(node)->sys->ExpDim!=0||CHILD3(node)->sys->ExpDim!=0){
								  errorHandler(7,node->val);
								  return;
							  }
							  node->sys->ExpType=CHILD1(node)->sys->ExpType;
							  break;
						  case 9:
							  if(checkExpError(CHILD2(node))){
								  return;
							  }
							  node->sys->ExpType=CHILD2(node)->sys->ExpType;
							  break;
						  case 10:
							  if(checkExpError(CHILD2(node))){
								  return;
							  }
							  if(!(CHILD2(node)->sys->ExpType!=_int&&CHILD2(node)->sys->ExpType!=_float)){
								  errorHandler(7,node->val);
							  }
							  node->sys->ExpType=CHILD2(node)->sys->ExpType;
							  break;
						  case 11:
							  if(checkExpError(CHILD2(node))){
								  return;
							  }
							  if(!CHILD2(node)->sys->ExpType!=_int){
								  errorHandler(7,node->val);
							  }
							  node->sys->ExpType=_int;
							  break;
						  case 12:
						  case 13:{
									  functionItem *item=functionSearch(CHILD1(node)->text);
									  if(item==0){
										  varibleItem *vitem=varibleSearch(CHILD1(node)->text);
										  if(vitem!=0){
											  errorHandler(11,CHILD1(node)->val);
											  return;
										  }
										  errorHandler(2,CHILD1(node)->val);
										  return;
									  }
									  if(item->length!=0){
										  if(CHILD3(node)->type!=Args){
											  errorHandler(9,node->val);
											  return;
										  }
										  treeNode *args=node;
										  for(int i=0;i<item->length;i++){
											  if(CHILD2(args)==0){
												  errorHandler(9,args->val);
												  return;
											  }
											  args=CHILD3(args);
											  if(item->parameters[i]->type!=CHILD1(args)->sys->ExpType){
												  errorHandler(9,args->val);
												  return;
											  }
										  }
										  if(CHILD2(args)!=0){
											  errorHandler(9,args->val);
											  return;
										  }
									  }
									  node->sys->ExpType=item->returnType;
									  break;
								  }
						  case 14:{
									  if(checkExpError(CHILD1(node))||checkExpError(CHILD3(node))){
										  return;
									  }
									  if(CHILD1(node)->sys->ExpDim==0){
										  errorHandler(10,node->val);
										  return;
									  }
									  if(CHILD3(node)->sys->ExpType!=_int){
										  errorHandler(12,node->val);
										  return;
									  }
									  node->sys->ExpType=CHILD1(node)->sys->ExpType;
									  node->sys->ExpDim=CHILD1(node)->sys->ExpDim-1;
									  break;
								  }
						  case 15:{
									  if(checkExpError(CHILD1(node))){
										  return;
									  }
									  if(CHILD1(node)->sys->ExpType<2){
										  errorHandler(13,node->val);
										  return;
									  }
									  structItem *item=structGet(CHILD1(node)->sys->ExpType);
									  varibleItem *vitem=searchField(item,CHILD3(node)->text);
									  if(vitem==0){
										  errorHandler(14,node->val);
										  return;
									  }
									  node->sys->ExpType=vitem->type;
									  node->sys->ExpDim=vitem->arrayDim;
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
									  node->sys->ExpType=item->type;
									  node->sys->ExpDim=item->arrayDim;
									  break;
								  }
						  case 17:
								  node->sys->ExpType=_int;
								  node->sys->ExpDim=0;
								  break;
						  case 18:
								  node->sys->ExpType=_float;
								  node->sys->ExpDim=0;
								  break;
					  }
					  break;
		case Stmt:
					  switch(node->genCount){
						  case 3:{
									 if(checkExpError(CHILD2(node))){
										 return;
									 }
									 treeNode *func=node;
									 while(func->type!=ExtDef){
										 func=func->parentNode;
									 }
									 functionItem *item=functionSearch(CHILD1(CHILD2(func))->text);
									 if(item!=0){
										 if(item->returnType!=CHILD2(node)->sys->ExpType){
											 errorHandler(8,node->val);
											 return;
										 }
									 }
									 else{
										 if(specifierRead(CHILD1(func))!=CHILD2(node)->sys->ExpType){
											 errorHandler(8,node->val);
											 return;

										 }
									 }
									 break;
								 }
						  case 4:
						  case 5:
						  case 6:
								 if(checkExpError(CHILD3(node))){
									 return;
								 }
								 if(CHILD3(node)->sys->ExpType!=_int){
									 errorHandler(7,CHILD3(node)->val);
								 }

					  }
					  break;
		case Program:
					  if(unimplementedFunctions!=0){
						  errorHandler(18,yylineno-1);
					  }
					  break;
	}
	return;
}

