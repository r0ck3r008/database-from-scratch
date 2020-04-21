// TODO
// Start making the tree by first adding the sel operations in the list.
// The start adding join operations, in incremental join order
#include"qp_tree.h"

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
