#include"qp_tree.h"

using namespace std;

void Qptree :: process(struct TableList *tables)
{
	struct TableList *curr=tables;
	while(curr!=NULL) {
		string tbl=string(curr->tableName);
		Schema *sch=new Schema(this->catalog_file, curr->tableName);
		this->relations.insert(pair<string, Schema *>(tbl, sch));
		if(curr->aliasAs!=NULL) {
			this->s->CopyRel(curr->tableName, curr->aliasAs);
			string tbl_as=string(curr->aliasAs);
			this->relations.insert(pair<string, Schema *>(tbl_as,
									sch));
		}
		curr=curr->next;
	}
}

void Qptree :: process(struct OrList *o_list)
{
	struct ComparisonOp *cop=o_list->left;
	if(cop->left->code==NAME && cop->right->code==NAME)
		o_list->is_join=1;
	else
		o_list->is_join=0;

	if(o_list->rightOr!=NULL)
		this->process(o_list->rightOr);

}

void Qptree :: mk_op_list(struct AndList *a_list)
{
	if(a_list->rightAnd!=NULL)
		a_list->rightAnd=NULL;

	if(a_list->left!=NULL)
		this->process(a_list->left);

	struct operation *op=new operation;
	op->a_list=a_list;
}

void Qptree :: process(struct AndList *a_list)
{
	if(a_list->rightAnd!=NULL)
		this->process(a_list->rightAnd);

	this->mk_op_list(a_list);
}
