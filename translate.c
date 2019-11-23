#include "translate.h"

IRStmtList *head;

void combineCode(treeNode *node){
	IRStmtList *newList=translate(node);
	head=catStmtList(head,newList);
}

IRStmtList *translate(treeNode *node){
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

IRStmtList *translateExp(IRVar retVar,treeNode *node);
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
	
}
IRStmtList *translateWhile(treeNode *node){

}
IRStmtList *translateCond(treeNode *node,IRVar *Lt,IRVar *Lf){
	switch(node->genCount){
		case 2://AND
			IRVar *label1=newLabelIRVar();
			IRStmt *LS1=newStmt(_LABE,NULL,label1,NULL);
			IRStmt *LSL1=newStmtList(LS1);
			IRStmtList *list1=translateCond(CHILD1(node),label1,Lf);
			IRStmtList *list2-translateCond(CHILD3(node),Lt,Lf);
			IRStmtList *ret=catStmtList(list1,LSL1);
			ret=catStmtList(ret,list2);
			return ret;
			break;
		case 3://OR
			IRVar *label1=newLabelIRVar();
			IRStmt *LS1=newStmt(_LABE,NULL,label1,NULL);
			IRStmt *LSL1=newStmtList(LS1);
			IRStmtList *list1=translateCond(CHILD1(node),Lt,label1);
			IRStmtList *list2-translateCond(CHILD3(node),Lt,Lf);
			IRStmtList *ret=catStmtList(list1,LSL1);
			ret=catStmtList(ret,list2);
			return ret;
			break;
		case 4://RELOP
			int type;
			IRVar *t1=newTempVar();
			IRVar *t2=newTempVar();
			IRStmtList *list1=translateExp(t1,CHILD1(node));
			IRStmtList *list2=translateExp(t2,CHILD3(node));

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
			IRStmt *condS=newStmt(type,Lt,t1,t2);
			IRStmtList *condSL=newStmtList(condS);

			IRStmt *gotoS=newStmt(_GOTO,Lf,NULL,NULL);
			IRStmtList *gotoSL=newStmtList(gotoS);

			IRStmtList *ret=catStmtList(list1,list2);
			ret=catStmtList(ret,condSL);
			ret=catStmtList(ret,gotoSL);
			return ret;

			break;
		case 11://NOT
			return translateCond(CHILD2(node),Lf,Lt);
			break;
		default:

			break;
	}
}
