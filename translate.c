#include "translate.h"

IRStmtList *head;

void combineCode(treeNode *node){
	if(node->visited==0){
		IRStmtList *newList=translate(node);
		head=catStmtList(head,newList);
	}
}

IRStmtList *translate(treeNode *node){
	node->visited=1;
	switch(node->type){
		case Stmt:
			switch(node->genCount){
				case 1:
					return translateExpStmt(node);
					break;
				case 2:
					return NULL;
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
			break;
	}
}

IRStmtList *translateExp(IRVar *retVar,treeNode *node){
	return null;	
}

IRStmtList *translateExpStmt(treeNode *node){
	return translateExp(CHILD1(node));
}
IRStmtList *translateReturn(treeNode *node){
	IRVar *retVar=newTempVar();
	IRStmt *retStmt=newStmt(_RETU,retVar,NULL);
	IRStmtList *newList=newStmtList(retStmt);
	IRStmtList *expList=newStmtList(retVar,CHILD2(node));
	return catStmtList(expList,newList);
}
IRStmtList *translateIf(treeNode *node){
	IRVar l1=newLabelIRVar();
	IRVar l2=newLabelIRVar();
	IRStmtList *condSL=translateCond(CHILD3(node),l1,l2);
	IRStmtList *list1=translate(CHILD5(node));
	IRStmt *LS1=newStmt(_LABE,NULL,l1,NULL);
	IRStmtList *LSL1=newStmtList(LS1);
	IRStmt *LS2=newStmt(_LABE,NULL,l2,NULL);
	IRStmtList *LSL2=newStmtList(LS2);
	IRStmtList *ret=catStmtList(condSL,LSL1);
	ret=catStmtList(ret,list1);
	if(node->genCount==5){
		IRVar l3=newLabelIRVar();
		IRStmt *LS3=newStmt(_LABE,NULL,l3,NULL);
		IRStmtList *LSL3=newStmtList(LS3);
		IRStmt *gotoLS3=newStmt(_GOTO,NULL,l3,NULL);
		IRStmt *gotoLSL3=newStmtList(gotoL3);
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
	IRVar l1=newLabelIRVar();
	IRVar l2=newLabelIRVar();
	IRVar l3=newLabelIRVar();
	IRStmt *LS1=newStmt(_LABE,NULL,l1,NULL);
	IRStmtList *LSL1=newStmtList(LS1);
	IRStmt *LS2=newStmt(_LABE,NULL,l2,NULL);
	IRStmtList *LSL2=newStmtList(LS2);
	IRStmt *LS3=newStmt(_LABE,NULL,l3,NULL);
	IRStmtList *LSL3=newStmtList(LS3);
	IRStmt *gotoLS1=newStmt(_GOTO,NULL,l1,NULL);
	IRStmt *gotoLSL1=newStmtList(gotoL1);
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
	IRVar *l1;
	IRVar *l2;
	IRVar *l3;
	IRVar *t1;
	IRVar *t2;
	IRStmt *LS1;
	IRStmt *LS2;
	IRStmt *LS3;
	IRStmt *gotoS;
	IRStmtList *LSL1;
	IRStmtList *LSL2;
	IRStmtList *LSL3;
	IRStmtList *gotoSL;
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
			int type;
			t1=newTempVar();
			t2=newTempVar();
			list1=translateExp(t1,CHILD1(node));
			list2=translateExp(t2,CHILD3(node));

			if(strcm(CHILD2(node)->text,">")){
				type=_IFL;
			}	
			else if(strcm(CHILD2(node)->text,">=")){
				type=_IFLE;
			}	
			else if(strcm(CHILD2(node)->text,"==")){
				type=_IFE;
			}	
			else if(strcm(CHILD2(node)->text,"!=")){
				type=_IFNE;
			}	
			else if(strcm(CHILD2(node)->text,"<=")){
				type=_IFSE;
			}	
			else if(strcm(CHILD2(node)->text,"<")){
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
