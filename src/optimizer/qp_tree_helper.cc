#include"qp_tree.h"

using namespace std;

bool op_comp_join :: operator()(operation *l, operation *r)
{
	//min queue
	return (l->cost > r->cost);
}

void Qptree :: process(struct TableList *tbls)
{
	struct TableList *curr=tbls;
	while(curr!=NULL) {
		this->s->addRel(curr->tableName, curr->aliasAs);
		Schema *sch=this->c->snap(curr->tableName);
		string rname;
		if(curr->aliasAs==NULL)
			rname=string(curr->tableName);
		else
			rname=string(curr->aliasAs);
		tableInfo *tbl=new tableInfo;
		tbl->sch=sch;
		tbl->add_sel(NULL, 0);
		this->relations.insert(pair<string, tableInfo *>(rname, tbl));
		curr=curr->next;
	}
}

void Qptree :: process(struct AndList *alist, struct OrList *olist)
{
	if(olist==NULL && alist->left!=NULL) {
		this->process(alist, alist->left);
	} else {

	}

	if(olist!=NULL && olist->rightOr!=NULL)
		this->process(alist, olist->rightOr);
}

void Qptree :: mk_ops(struct AndList *alist)
{
	//traverse to right most and make an op
	if(alist->rightAnd!=NULL)
		mk_ops(alist->rightAnd);

	this->process(alist, NULL);
}
