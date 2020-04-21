// TODO
// Start making the tree by first adding the sel operations in the list.
// The start adding join operations, in incremental join order
#include"qp_tree.h"

operation :: operation()
{
	this->l_child=NULL;
	this->r_child=NULL;
	this->parent=NULL;
	this->type=no_op;
}
operation :: operation(struct AndList *a_list, Qptree *ref)
{
	this->a_list=a_list;
	if(a_list->rightAnd!=NULL)
		//assuming all the right Ands have already been dealt with
		a_list->rightAnd=NULL;

	//max 16 relations
	char **rels=new char *[16];
	int indx=0;
	ref->process(a_list, NULL, rels, &indx);
	this->cost=ref->s->Estimate(a_list, rels, indx);
	//assuming atomic operations per AndList
	for(int i=0; i<indx; i++) {
		string r_name=string(rels[i]);
		auto itr=ref->relations.find(r_name);
		if(indx==2) {
			itr->second.join_order++;
			this->type=join_op;
			ref->join_queue.push(this);
		} else {
			itr->second.sel_order++;
			this->type=sel_any;
			itr->second.sel_queue.push(this);
		}
	}
	for(int i=0; i<indx; i++)
		delete[] rels[i];
	delete[] rels;
}
operation :: ~operation(){}

bool operation_comp :: operator()(operation *l, operation *r)
{
	return (l->cost > r->cost);
}

tableInfo :: tableInfo()
{
	this->join_order=0;
	this->sel_order=0;
}
tableInfo :: ~tableInfo(){}

Qptree :: Qptree(char *stat_fname, char *catalog_file)
{
	this->s=new Statistics;
	this->s->Read(stat_fname);
	this->catalog_file=catalog_file;
}

Qptree :: ~Qptree()
{
	delete this->s;
	for(auto &i: this->relations)
		delete i.second.sch;
}

void Qptree :: process(struct query *q)
{
	this->process(q->tables);
	this->process(q->boolean);
}
