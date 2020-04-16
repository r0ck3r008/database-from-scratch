#include"qp_tree.h"

void QPTree :: process(struct TableList *tables)
{
	struct TableList *curr=tables;
	while(curr!=NULL) {
		this->s->CopyRel(curr->tableName, curr->aliasAs);
		curr=curr->next;
	}
}

void QPTree :: process(AndList *a_list, OrList *o_list)
{
	if(o_list==NULL && a_list->left!=NULL)
		//Move down from AND to OR
		this->process(a_list, a_list->left);
	else if(o_list!=NULL)
		;

	if(o_list!=NULL && o_list->rightOr!=NULL)
		//Move right from OR to OR
		this->process(a_list, o_list->rightOr);

	if(a_list->rightAnd!=NULL)
		//Move right from AND to AND
		this->process(a_list->rightAnd, NULL);
}
