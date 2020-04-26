#include"qp_tree.h"

void tableInfo :: add_sel(struct AndList *alist, double cost)
{
	type_flag type;
	struct operation *op;
	if(alist==NULL) {
		op=new operation(self_f);
		op->lid=-2;
		op->cost=0;
		op->self.alist=NULL;
		this->sel_que.push(op);
	} else {
		if(this->sel_que.size()==1) {
			op=this->sel_que.top();
			op->self.alist=alist;
			op->cost=cost;
		} else {
			op=new operation(selp_f);
			this->sel_que.push(op);
		}
	}
}


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
