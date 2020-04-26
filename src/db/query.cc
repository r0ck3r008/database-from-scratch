#include<stdlib.h>

#include"query.h"

query :: query(struct FuncOperator *func, struct TableList *tbls,
		struct AndList *a_list, struct NameList *grp_atts,
		struct NameList *sel_atts, int dis_att, int dis_func)
{
	this->func=func;
	this->tbls=tbls;
	this->a_list=a_list;
	this->grp_atts=grp_atts;
	this->sel_atts=sel_atts;
	this->dis_att=dis_att;
	this->dis_func=dis_func;
}

query :: ~query()
{
	del_name_list(this->grp_atts);
	del_name_list(this->sel_atts);
	del_tbl_list(this->tbls);
	del_and_list(this->a_list, NULL);
	del_func_list(this->func);
}

void del_name_list(struct NameList *n_list)
{
	struct NameList *curr=n_list->next;
	//not sure if char *name is malloc'd
	if(curr!=NULL) {
		n_list->next=curr->next;
		free(curr);
		del_name_list(n_list);
	} else {
		free(n_list);
		n_list=NULL;
	}
}

void del_tbl_list(struct TableList *tbls)
{
	struct TableList *curr=tbls->next;
	//not sure if char *name is malloc'd
	if(curr!=NULL) {
		tbls->next=curr->next;
		free(curr);
		del_tbl_list(tbls);
	} else {
		free(tbls);
		tbls=NULL;
	}

}

void del_and_list(struct AndList *a_list, struct OrList *o_list)
{
	if(o_list==NULL && a_list->left!=NULL) {
		del_and_list(a_list, a_list->left);
	} else if(o_list!=NULL) {
		struct ComparisonOp *cop=o_list->left;
		free(cop->left);
		free(cop->right);
		free(cop);
	}

	if(o_list!=NULL && o_list->rightOr!=NULL) {
		del_and_list(a_list, o_list->rightOr);
		free(o_list->rightOr);
		free(o_list);
		o_list=NULL;
	} else if(o_list==NULL && a_list!=NULL && a_list->rightAnd!=NULL) {
		del_and_list(a_list->rightAnd, NULL);
		free(a_list->rightAnd);
		free(a_list);
		a_list=NULL;
	}
}

void del_func_list(struct FuncOperator *f_list)
{
	if(f_list!=NULL && f_list->leftOperator!=NULL)
		del_func_list(f_list->leftOperator);

	if(f_list!=NULL && f_list->leftOperand!=NULL)
		free(f_list->leftOperand);

	if(f_list!=NULL && f_list->right!=NULL)
		del_func_list(f_list->right);

	if(f_list!=NULL) {
		free(f_list);
		f_list=NULL;
	}
}
