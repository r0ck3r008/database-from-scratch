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

struct operation *tableInfo :: dispense_sel(Qptree *qpt)
{
	struct operation *curr_op=NULL;
	while(!(this->sel_que.empty())) {
		int pipe=0;
		Pipe *p=qpt->dispense_pipe(&pipe);
		if(curr_op==NULL) {
			curr_op=this->sel_que.top();
			curr_op->pid=pipe;
			curr_op->self.opipe=p;
		} else {
			struct operation *op=this->sel_que.top();
			curr_op->parent=op;
			op->lchild=curr_op;
			if(curr_op->type & self_f)
				op->selp.ipipe=curr_op->self.opipe;
			else
				op->selp.ipipe=curr_op->selp.opipe;
			op->lid=curr_op->pid;
			op->selp.opipe=p;
			op->pid=pipe;
			curr_op=op;
		}
	}

	return curr_op;
}

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
