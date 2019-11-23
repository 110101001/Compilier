#include "translate.h"

IRStmtList *head;

void combineCode(treeNode *node){
	if(node->visited==1){
		return;
	}
	IRStmtList *newList=translateStmtList(node);
	head=catStmtList(head,newList);
}

IRStmtList *translateStmtList(treeNode *node){
	if(node->type==StmtList){
		node->visited=1;
		IRStmtList *newList=NULL;
		while(node!=0){
			newList=catStmtList(newList,translate(CHILD1(node)));
			node=CHILD2(node);
		}
		return newList;
	}
	else if(node->type==DecList){
		node->visited=1;
		IRStmtList *newList=NULL;
		while(node!=0){
			treeNode *decNode=CHILD2(CHILD1(node));



			node=CHILD2(node);
		}
		return newList;
	}
	else {
		return NULL;
	}
}

IRStmtList *translate(treeNode *node){
	if(node->visited==1){
		return NULL;
	}
	node->visited=1;
	if(node->type==Stmt){
		switch(node->genCount){
			case 1:
				return translateExpStmt(node);
				break;
			case 2:
				return catStmtList(translateStmtList(CHILD2(CHILD1(node))),translateStmtList(CHILD3(CHILD1(node))));
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
			v1=newVaribleIRVar(CHILD1(CHILD1(node))->text);
			t1=newTempIRVar();
			list1=translateExp(t1,CHILD3(node));
			assignS=newStmt(_ASSI,v1,t1,NULL);
			assignSL=newStmtList(assignS);
			mainS=newStmt(_ASSI,retVar,v1,NULL);
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
	IRStmtList *list1=translate(CHILD5(node));
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
		IRStmtList *list2=translate(CHILD7(node));
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
	IRStmtList *list1=translate(CHILD5(node));
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
