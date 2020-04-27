#include"tableinfo.h"

bool op_comp_sel :: operator()(operation *l, operation *r)
{
	//max queue which always keeps select file op at the top
	if(l->type & self_f)
		return false;
	else
		return(l->cost < r->cost);
}

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
