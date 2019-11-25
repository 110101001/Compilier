#include "translate.h"
#include "symbolTable.h"

IRStmtList *head;

void combineCode(treeNode *node){
	IRStmtList *newList=translate(node);
	head=catStmtList(head,newList);
}

IRStmtList *translate(treeNode *node){
	if(node->visited==1){
		return NULL;
	}
	switch(node->type){
		case StmtList:
			return translateStmtList(node);
			break;
		case ExtDef:
			return translateExtDef(node);
			break;
		case DefList:
			return translateDefList(node);
			break;
	}
	return NULL;
}

IRStmtList *translateDec(treeNode *node){
	if(node->type!=Dec){
		return NULL;
	}
	if(node->genCount==2){
		IRVar *v1=newVaribleIRVar(CHILD1(CHILD1(node))->text);
		IRVar *t1=newTempIRVar();
		IRStmtList *list1=translateExp(t1,CHILD3(node));
		IRStmt *assignS=newStmt(_ASSI,v1,t1,NULL);
		IRStmtList *assignSL=newStmtList(assignS);
		IRStmtList *ret=catStmtList(list1,assignSL);
		return ret;
	}
	else if(CHILD1(node)->genCount==2){
		return translateArrayDec(CHILD1(node));
	}
	return NULL;
}

IRStmtList *translateDefList(treeNode *node){
	node->visited=1;
	IRStmtList *ret=NULL;
	if(node->type==DefList){
		treeNode *DefL=node;
		while(DefL!=0){
			treeNode *DecL=CHILD2(CHILD1(DefL));
			while(1){
				ret=catStmtList(ret,translateDec(CHILD1(DecL)));
				if(CHILD2(DecL)==0){
					break;
				}
				DecL=CHILD3(DecL);
			}
			DefL=CHILD2(DefL);
		}
	}
	return ret;
}

IRStmtList *translateExtDef(treeNode *node){
	if(node->visited==1){
		return NULL;
	}
	node->visited=1;
	if(node->type!=ExtDef){
		return NULL;
	}
	switch(node->genCount){
		case 3://Functiion Def
			return translateFunction(node);
			break;
	}
	return NULL;
}

IRStmtList *translateFunction(treeNode *node){
	IRVar *f1;
	IRStmt *FDS;
	IRStmtList *FDSL;
	IRStmtList *list1;
	IRStmtList *PSL=NULL;
	IRStmtList *ret;
	f1=newVaribleIRVar(CHILD1(CHILD2(node))->text);
	FDS=newStmt(_FUNC,NULL,f1,NULL);
	FDSL=newStmtList(FDS);
	list1=translateCompSt(CHILD3(node));

	functionItem *item=functionSearch(f1->name);
	for(int i=0;i<item->length;i++){
		IRVar *v1=newVaribleIRVar(item->parameters[i]->name);
		IRStmt *PS=newStmt(_PARA,v1,NULL,NULL);
		PSL=catStmtList(PSL,newStmtList(PS));
	}
	ret=catStmtList(FDSL,PSL);
	ret=catStmtList(ret,list1);
	return ret;
}

IRStmtList *translateArgs(treeNode *node){
	treeNode *present=node;
	IRStmtList *args=NULL;
	IRStmtList *argExps=NULL;
	IRStmtList *ret=NULL;
	if(present->type==Args){
		while(1){
			IRVar *argVar=newTempIRVar();
			argExps=catStmtList(argExps,translateExp(argVar,CHILD1(present)));
			args=catStmtList(args,newStmtList(newStmt(_ARG,NULL,argVar,NULL)));
			if(CHILD2(present)==0){
				break;
			}
			present=CHILD3(present);
		}
	}
	ret=catStmtList(argExps,args);
	return ret;
}

IRStmtList *translateCall(IRVar *retVar,treeNode *node){
	IRVar *f1=newVaribleIRVar(CHILD1(node)->text);
	IRStmt *FCS=newStmt(_CALL,retVar,f1,NULL);
	IRStmtList *FCSL=newStmtList(FCS);
	IRStmtList *expList=translateArgs(CHILD3(node));
	IRStmtList *ret=NULL;
	if(strcm(CHILD1(node)->text,"read")==0){
		IRStmt *readS=newStmt(_READ,retVar,NULL,NULL);
		IRStmtList *readSL=newStmtList(readS);
		return readSL;
	}
	else if(strcm(CHILD1(node)->text,"write")==0){
		IRStmtList *p=expList;
		while(p->next!=0){
			p=p->next;
		}
		IRVar *writeVar=p->stmt->arg1;
		removeNextStmt(p);
		IRStmt *writeS=newStmt(_WRIT,NULL,writeVar,NULL);
		IRStmtList *writeSL=newStmtList(writeS);
		ret=catStmtList(expList,writeSL);
		return ret;
	}
	ret=catStmtList(expList,FCSL);
	return ret;
}

IRStmtList *translateCompSt(treeNode *node){
	if(CHILD2(node)->type==DefList){
		return catStmtList(translateDefList(CHILD2(node)),translateStmtList(CHILD3(node)));
	}
	else{
		return translateStmtList(CHILD2(node));
	}
}

IRStmtList *translateStmtList(treeNode *node){
	if(node->visited==1){
		return NULL;
	}
	node->visited=1;
	if(node->type==StmtList){
		node->visited=1;
		IRStmtList *newList=NULL;
		newList=catStmtList(newList,translateStmt(CHILD1(node)));
		if(CHILD2(node)!=0){
			newList=catStmtList(newList,translateStmtList(CHILD2(node)));
		}
		return newList;
	}
	else {
		return NULL;
	}
}

IRStmtList *translateStmt(treeNode *node){
	if(node->type==Stmt){
		switch(node->genCount){
			case 1:
				return translateExpStmt(node);
				break;
			case 2:
				return translateCompSt(CHILD1(node));
				break;
			case 3:
				return translateReturn(node);
				break;
			case 4:
			case 5:
				return translateIf(node);
				break;
			case 6:
				return translateWhile(node);
				break;
		}
	}
}

IRStmtList *translateExp(IRVar *retVar,treeNode *node){
	IRVar *v1;
	IRVar *t1;
	IRVar *t2;
	IRVar *c1;
	IRVar *c2;
	IRVar *l1;
	IRVar *l2;
	IRStmt *mainS;
	IRStmt *assignS;
	IRStmt *LS1;
	IRStmt *LS2;
	IRStmtList *mainSL;
	IRStmtList *assignSL;
	IRStmtList *LSL1;
	IRStmtList *LSL2;
	IRStmtList *list1;
	IRStmtList *list2;
	IRStmtList *ret;
	switch(node->genCount){
		case 1:
			t1=newTempIRVar();
			list1=translateExp(t1,CHILD3(node));
			if(CHILD1(node)->genCount==16){
				v1=newVaribleIRVar(CHILD1(CHILD1(node))->text);
				assignS=newStmt(_ASSI,v1,t1,NULL);
				assignSL=newStmtList(assignS);
			}
			else{
				assignSL=translateArray(NULL,t1,CHILD1(node));
			}
			mainS=newStmt(_ASSI,retVar,t1,NULL);
			mainSL=newStmtList(mainS);
			ret=catStmtList(list1,assignSL);
			ret=catStmtList(ret,mainSL);
			return ret;
			break;
		case 2:
		case 3:
		case 4:
		case 11:
			l1=newLabelIRVar();
			l2=newLabelIRVar();
			c1=newNumIRVar(0);
			c2=newNumIRVar(1);
			LS1=newStmt(_LABE,NULL,l1,NULL);
			LSL1=newStmtList(LS1);
			LS2=newStmt(_LABE,NULL,l2,NULL);
			LSL2=newStmtList(LS2);
			list1=translateCond(node,l1,l2);
			assignS=newStmt(_ASSI,retVar,c1,NULL);
			assignSL=newStmtList(assignS);
			mainS=newStmt(_ASSI,retVar,c2,NULL);
			mainSL=newStmtList(mainS);
			ret=catStmtList(assignSL,list1);
			ret=catStmtList(ret,LSL1);
			ret=catStmtList(ret,mainSL);
			ret=catStmtList(ret,LSL2);
			return ret;
			break;
		case 5:
			t1=newTempIRVar();
			t2=newTempIRVar();
			list1=translateExp(t1,CHILD1(node));
			list2=translateExp(t2,CHILD3(node));
			mainS=newStmt(_ADD,retVar,t1,t2);
			mainSL=newStmtList(mainS);
			ret=catStmtList(list1,list2);
			ret=catStmtList(ret,mainSL);
			return ret;
		case 6:
			t1=newTempIRVar();
			t2=newTempIRVar();
			list1=translateExp(t1,CHILD1(node));
			list2=translateExp(t2,CHILD3(node));
			mainS=newStmt(_SUB,retVar,t1,t2);
			mainSL=newStmtList(mainS);
			ret=catStmtList(list1,list2);
			ret=catStmtList(ret,mainSL);
			return ret;
			break;
		case 7:
			t1=newTempIRVar();
			t2=newTempIRVar();
			list1=translateExp(t1,CHILD1(node));
			list2=translateExp(t2,CHILD3(node));
			mainS=newStmt(_MULT,retVar,t1,t2);
			mainSL=newStmtList(mainS);
			ret=catStmtList(list1,list2);
			ret=catStmtList(ret,mainSL);
			return ret;
			break;
		case 8:
			t1=newTempIRVar();
			t2=newTempIRVar();
			list1=translateExp(t1,CHILD1(node));
			list2=translateExp(t2,CHILD3(node));
			mainS=newStmt(_DIVI,retVar,t1,t2);
			mainSL=newStmtList(mainS);
			ret=catStmtList(list1,list2);
			ret=catStmtList(ret,mainSL);
			return ret;
			break;
		case 9:
			return translateExp(retVar,CHILD2(node));
		case 10:
			c1=newNumIRVar(0);
			t1=newTempIRVar();
			list1=translateExp(t1,CHILD2(node));
			mainS=newStmt(_SUB,retVar,c1,t1);
			mainSL=newStmtList(mainS);
			ret=catStmtList(list1,mainSL);
			return ret;
			break;
		case 12:
		case 13:
			return translateCall(retVar,node);
			break;
		case 14:
			return translateArray(retVar,NULL,node);
			break;
		case 16:
			v1=newVaribleIRVar(CHILD1(node)->text);
			mainS=newStmt(_ASSI,retVar,v1,NULL);
			mainSL=newStmtList(mainS);
			ret=mainSL;
			return ret;
			break;
		case 17:
			t1=newNumIRVar(CHILD1(node)->val);
			mainS=newStmt(_ASSI,retVar,t1,NULL);
			mainSL=newStmtList(mainS);
			ret=mainSL;
			return ret;
			break;
			
	}
	return NULL;	
}

IRStmtList *translateExpStmt(treeNode *node){
	IRVar *temp=newTempIRVar();
	return translateExp(temp,CHILD1(node));
}

IRStmtList *translateReturn(treeNode *node){
	IRVar *retVar=newTempIRVar();
	IRStmt *retStmt=newStmt(_RETU,NULL,retVar,NULL);
	IRStmtList *newList=newStmtList(retStmt);
	IRStmtList *expList=translateExp(retVar,CHILD2(node));
	return catStmtList(expList,newList);
}

IRStmtList *translateIf(treeNode *node){
	IRVar *l1=newLabelIRVar();
	IRVar *l2=newLabelIRVar();
	IRStmtList *condSL=translateCond(CHILD3(node),l1,l2);
	IRStmtList *list1=translateStmt(CHILD5(node));
	IRStmt *LS1=newStmt(_LABE,NULL,l1,NULL);
	IRStmtList *LSL1=newStmtList(LS1);
	IRStmt *LS2=newStmt(_LABE,NULL,l2,NULL);
	IRStmtList *LSL2=newStmtList(LS2);
	IRStmtList *ret=catStmtList(condSL,LSL1);
	ret=catStmtList(ret,list1);
	if(node->genCount==5){
		IRVar *l3=newLabelIRVar();
		IRStmt *LS3=newStmt(_LABE,NULL,l3,NULL);
		IRStmtList *LSL3=newStmtList(LS3);
		IRStmt *gotoLS3=newStmt(_GOTO,l3,NULL,NULL);
		IRStmtList *gotoLSL3=newStmtList(gotoLS3);
		IRStmtList *list2=translateStmt(CHILD7(node));
		ret=catStmtList(ret,gotoLSL3);
		ret=catStmtList(ret,LSL2);
		ret=catStmtList(ret,list2);
		ret=catStmtList(ret,LSL3);
	}
	else{
		ret=catStmtList(ret,LSL2);
	}
	return ret;
}

IRStmtList *translateWhile(treeNode *node){
	IRVar *l1=newLabelIRVar();
	IRVar *l2=newLabelIRVar();
	IRVar *l3=newLabelIRVar();
	IRStmt *LS1=newStmt(_LABE,NULL,l1,NULL);
	IRStmtList *LSL1=newStmtList(LS1);
	IRStmt *LS2=newStmt(_LABE,NULL,l2,NULL);
	IRStmtList *LSL2=newStmtList(LS2);
	IRStmt *LS3=newStmt(_LABE,NULL,l3,NULL);
	IRStmtList *LSL3=newStmtList(LS3);
	IRStmt *gotoLS1=newStmt(_GOTO,l1,NULL,NULL);
	IRStmtList *gotoLSL1=newStmtList(gotoLS1);
	IRStmtList *condSL=translateCond(CHILD3(node),l2,l3);
	IRStmtList *list1=translateStmt(CHILD5(node));
	IRStmtList *ret=catStmtList(LSL1,condSL);
	ret=catStmtList(ret,LSL2);
	ret=catStmtList(ret,list1);
	ret=catStmtList(ret,gotoLSL1);
	ret=catStmtList(ret,LSL3);
	return ret;
}

IRStmtList *translateCond(treeNode *node,IRVar *Lt,IRVar *Lf){
	int type;
	IRVar *l1;
	IRVar *l2;
	IRVar *l3;
	IRVar *t1;
	IRVar *t2;
	IRVar *c1;
	IRStmt *LS1;
	IRStmt *LS2;
	IRStmt *LS3;
	IRStmt *gotoS;
	IRStmt *condS;
	IRStmtList *LSL1;
	IRStmtList *LSL2;
	IRStmtList *LSL3;
	IRStmtList *gotoSL;
	IRStmtList *condSL;
	IRStmtList *list1;
	IRStmtList *list2;
	IRStmtList *ret;
	switch(node->genCount){
		case 2://AND
			l1=newLabelIRVar();
			LS1=newStmt(_LABE,NULL,l1,NULL);
			LSL1=newStmtList(LS1);
			list1=translateCond(CHILD1(node),l1,Lf);
			list2-translateCond(CHILD3(node),Lt,Lf);
			ret=catStmtList(list1,LSL1);
			ret=catStmtList(ret,list2);
			return ret;
			break;
		case 3://OR
			l1=newLabelIRVar();
			LS1=newStmt(_LABE,NULL,l1,NULL);
			LSL1=newStmtList(LS1);
			list1=translateCond(CHILD1(node),Lt,l1);
			list2-translateCond(CHILD3(node),Lt,Lf);
			ret=catStmtList(list1,LSL1);
			ret=catStmtList(ret,list2);
			return ret;
			break;
		case 4://RELOP
			t1=newTempIRVar();
			t2=newTempIRVar();
			list1=translateExp(t1,CHILD1(node));
			list2=translateExp(t2,CHILD3(node));

			if(strcm(CHILD2(node)->text,">")==0){
				type=_IFL;
			}	
			else if(strcm(CHILD2(node)->text,">=")==0){
				type=_IFLE;
			}	
			else if(strcm(CHILD2(node)->text,"==")==0){
				type=_IFE;
			}	
			else if(strcm(CHILD2(node)->text,"!=")==0){
				type=_IFNE;
			}	
			else if(strcm(CHILD2(node)->text,"<=")==0){
				type=_IFSE;
			}	
			else if(strcm(CHILD2(node)->text,"<")==0){
				type=_IFS;
			}	
			condS=newStmt(type,Lt,t1,t2);
			condSL=newStmtList(condS);

			gotoS=newStmt(_GOTO,Lf,NULL,NULL);
			gotoSL=newStmtList(gotoS);

			ret=catStmtList(list1,list2);
			ret=catStmtList(ret,condSL);
			ret=catStmtList(ret,gotoSL);
			return ret;

			break;
		case 11://NOT
			return translateCond(CHILD2(node),Lf,Lt);
			break;
		default:
			t1=newTempIRVar();
			c1=newNumIRVar(0);
			list1=translateExp(t1,node);

			condS=newStmt(_IFNE,Lt,t1,c1);
			condSL=newStmtList(condS);

			gotoS=newStmt(_GOTO,Lf,NULL,NULL);
			gotoSL=newStmtList(gotoS);

			ret=catStmtList(list1,condSL);
			ret=catStmtList(ret,gotoSL);
			return ret;
			break;
	}
}

IRStmtList *translateArrayDec(treeNode *node){
	if(node->type!=VarDec){
		return NULL;
	}
	treeNode *dec=node;
	while(CHILD2(dec)!=0){
		dec=CHILD1(dec);
	}
	varibleItem *item=varibleSearch(CHILD1(dec)->text);
	int size=4;
	for(int i=0;i<item->arrayDim;i++){
		size=size*item->arrayLen[i];
	}
	IRVar *a1=newVaribleIRVar(CHILD1(dec)->text);
	IRVar *c1=newNumIRVar(size);
	IRStmt *DecS=newStmt(_DEC,a1,c1,NULL);
	IRStmtList *DecL=newStmtList(DecS);
	return DecL;
}
IRStmtList *translateArray(IRVar *retVar,IRVar *assignVar,treeNode *node){
	if(node->type!=Exp||node->genCount!=14){
		return NULL;
	}
	treeNode *p=node;
	while(CHILD1(p)!=0){
		p=CHILD1(p);
	}
	IRVar *a1=newVaribleIRVar(p->text);
	IRVar *t1=newTempIRVar();
	IRStmtList *locList;
	IRStmt *ADS=newStmt(_ADDR,t1,a1,NULL);
	IRStmtList *ADSL=newStmtList(ADS);
	locList=ADSL;
	varibleItem *item=varibleSearch(p->text);
	treeNode *current=p->parentNode->parentNode;
	for(int i=0;i<item->arrayDim;i++){
		int unitSize=4;
		for(int j=i+1;j<item->arrayDim;j++){
			unitSize=unitSize*item->arrayLen[j];
		}
		IRVar *USV=newNumIRVar(unitSize);
		IRVar *index=newTempIRVar();
		IRStmtList *expList=translateExp(index,CHILD3(current));
		current=current->parentNode;
		locList=catStmtList(locList,expList);
		IRStmt *multS=newStmt(_MULT,index,index,USV);
		IRStmtList *multSL=newStmtList(multS);
		locList=catStmtList(locList,multSL);
		IRStmt *addS=newStmt(_ADD,t1,t1,index);
		IRStmtList *addSL=newStmtList(addS);
		locList=catStmtList(locList,addSL);
	}
	if(assignVar!=NULL){
		IRStmt *AS2=newStmt(_WMEM,NULL,t1,assignVar);
		IRStmtList *ASL2=newStmtList(AS2);
		locList=catStmtList(locList,ASL2);
	}
	if(retVar!=NULL){
		IRStmt *AS2=newStmt(_REFE,retVar,t1,NULL);
		IRStmtList *ASL2=newStmtList(AS2);
		locList=catStmtList(locList,ASL2);
	}
	return locList;
}
