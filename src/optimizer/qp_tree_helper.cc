#include"qp_tree.h"

using namespace std;

bool op_comp_join :: operator()(operation *l, operation *r)
{
	//min queue
	return (l->cost > r->cost);
}

bool op_comp_sel :: operator()(operation *l, operation *r)
{
	//max queue which always keeps select file op at the top
	if(l->type & self_f)
		return false;
	else
		return(l->cost < r->cost);
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
		tableInfo tbl;
		tbl.sch=sch;
		this->relations.insert(pair<string, tableInfo>(rname, tbl));
		curr=curr->next;
	}
}

void Qptree :: mk_ops(struct AndList *alist)
{
	//traverse to right most and
	if(alist->rightAnd!=NULL)
		mk_ops(alist->rightAnd);
}
