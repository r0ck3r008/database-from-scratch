#include"qp_tree.h"

void tableInfo :: add_sel()
{

}

Qptree :: Qptree(Catalog *cat)
{
	this->c=cat;
	this->s=new Statistics(cat);
}
Qptree :: ~Qptree()
{
	delete this->s;
}

void Qptree :: process(struct query *q)
{
	this->process(q->tbls);
	this->mk_ops(q->a_list);
}
