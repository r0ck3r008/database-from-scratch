// TODO
// Start making the tree by first adding the sel operations in the list.
// The start adding join operations, in incremental join order
#include"qp_tree.h"

operation :: operation()
{
	this->l_child=NULL;
	this->r_child=NULL;
	this->parent=NULL;
}
operation :: ~operation(){}

tableInfo :: tableInfo()
{
	this->join_order=0;
	this->sel_order=0;
}
tableInfo :: ~tableInfo(){}

bool operation_comp :: operator()(const operation &l, const operation &r)
{
	return (l.cost > r.cost);
}

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
