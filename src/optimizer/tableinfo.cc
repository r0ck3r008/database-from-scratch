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
	vector<tableInfo *> vec;
	vec.push_back(this);
	if(alist==NULL) {
		op=new operation(self_f, cost, vec);
		op->lid=-2;
		op->self.alist=NULL;
		this->sel_que.push(op);
	} else {
		if(this->sel_que.size()==1) {
			op=this->sel_que.top();
			op->self.alist=alist;
			op->cost=cost;
		} else {
			op=new operation(selp_f, cost, vec);
			this->sel_que.push(op);
		}
	}
}

struct operation *tableInfo :: dispense_sel(Qptree *qpt)
{
	struct operation *curr_op=NULL;
	while(!(this->sel_que.empty())) {
		if(curr_op==NULL) {
			int pipe=0;
			Pipe *p=qpt->dispense_pipe(&pipe);
			curr_op=this->sel_que.top();
			curr_op->pid=pipe;
			curr_op->add_pipe(parent_out, p);
		} else {
			struct operation *op=this->sel_que.top();
			mk_parent(qpt, op, curr_op, 0);
			curr_op=op;
		}
		this->sel_que.pop();
	}

	return curr_op;
}
