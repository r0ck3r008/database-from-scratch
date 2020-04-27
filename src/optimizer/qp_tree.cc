#include"qp_tree.h"

Qptree :: Qptree(Catalog *cat)
{
	this->c=cat;
	this->s=new Statistics(cat);
	this->curr_pipe_id=0;
	this->curr_pipe=NULL;
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

Pipe *Qptree :: dispense_pipe(int *pipe_id)
{
	*pipe_id=this->curr_pipe_id++;
	Pipe *p=new Pipe(1000);
	this->curr_pipe=p;
	return p;
}
