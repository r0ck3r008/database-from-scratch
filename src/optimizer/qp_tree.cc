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
	delete this->s;
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
			if(this->tree!=NULL)
				mk_parent(this, op, this->tree, 0);
			this->tree=op;
		}
	}
}

Pipe *Qptree :: dispense_pipe(int *pipe_id)
{
	*pipe_id=this->curr_pipe_id++;
	Pipe *p=new Pipe(1000);
	this->curr_pipe=p;
	return p;
}
