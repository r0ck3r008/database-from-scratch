#include<unistd.h>

#include"qp_tree.h"

Qptree :: Qptree(Catalog *cat)
{
	this->c=cat;
	this->s=new Statistics(cat);
	this->curr_pipe_id=0;
	this->curr_pipe=NULL;
	this->tree=NULL;
}
Qptree :: ~Qptree()
{
//	delete this->s;
}

void Qptree :: process(struct query *q)
{
	this->process(q->tbls);
	this->mk_ops(q->a_list);

	vector<operation *> vec;
	while(this->join_queue.size()!=0) {
		struct operation *op=this->join_queue.top();
		this->join_queue.pop();
		this->process(op, vec);
	}
	if(tree==NULL) {
		//in case of no join
		for(auto &i: relations) {
			struct operation *op=NULL;
			if((op=i.second->dispense_sel(this))==NULL) {
				cerr << "Error in fetching the select!\n";
				_exit(-1);
			}
			if(this->tree!=NULL) {
				mk_parent(this, op, this->tree, 0);
			}
			this->tree=op;
		}
	}

	this->process(q->grp_atts, q->func);
	this->execute(1);
}

Pipe *Qptree :: dispense_pipe(int *pipe_id)
{
	*pipe_id=this->curr_pipe_id++;
	Pipe *p=new Pipe(1000);
	this->curr_pipe=p;
	return p;
}

void Qptree :: execute(int flag)
{
	//add final pipe
	int pipe=0;
	Pipe *p=this->dispense_pipe(&pipe);
	this->tree->add_pipe(parent_out, p);
	this->tree->pid=pipe;
	//traverse as instructed
	this->tree->traverse(flag);
	//output if necessary
	if(flag) {
		Record tmp;
		Schema sch;
		if(this->tree->type & join_f) {
			sch=*(this->tree->join.schl) + *(this->tree->join.schr);
		} else if(this->tree->type & self_f) {
			sch=*(this->tree->self.sch);
		} else if((this->tree->type & grpby_f) ||
				(this->tree->type & sum_f)) {
			sch.myAtts[0].update("Double", Double);
			sch.numAtts=1;
		}
		int count=0;
		while(1) {
			int stat=this->curr_pipe->Remove(&tmp);
			if(!stat)
				break;
			tmp.Print(&sch);
			count++;
		}
		cout << "Counted: " << count << " records!\n";
	}
}
