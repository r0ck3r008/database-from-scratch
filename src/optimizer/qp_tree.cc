#include<string.h>
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

	if(q->dis_func)
		this->process();
	if(this->process(q->grp_atts, q->func)==0)
		this->process(q->func);
	if(q->dis_att)
		this->process();
	this->process(q->sel_atts);
	this->execute(q);
}

Pipe *Qptree :: dispense_pipe(int *pipe_id)
{
	*pipe_id=this->curr_pipe_id++;
	Pipe *p=new Pipe(1000);
	this->curr_pipe=p;
	return p;
}

void Qptree :: execute(struct query *q)
{
	int flag;
	if(q->output_var!=NULL) {
		if(!strcmp(q->output_var, "NONE"))
			flag=0;
		else if(!strcmp(q->output_var, "STDOUT"))
			flag=1;
		else
			flag=2;
	} else {
		flag=1;
	}
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
		if(this->tree->type & join_f)
			sch=*(this->tree->join.schl) + *(this->tree->join.schr);
		else if(this->tree->type & self_f)
			sch=*(this->tree->self.sch);
		else if(this->tree->type & grpby_f)
			sch=*(this->tree->grpby.sch);
		else if(this->tree->type & sum_f)
			sch=*(this->tree->sum.sch);
		else if(this->tree->type & dist_f)
			sch=*(this->tree->dist.sch);
		else if(this->tree->type & proj_f)
			sch=*(this->tree->proj.sch);
		if(flag==1) {
			int count=0;
			while(1) {
				int stat=this->curr_pipe->Remove(&tmp);
				if(!stat)
					break;
				tmp.Print(&sch);
				count++;
			}
			cout << "Counted: " << count << " records!\n";
		} else if(flag==2){
			WriteOut wr;
			FILE *f=NULL;
			if((f=fopen(q->output_var, "w"))==NULL) {
				cerr << "Error in opening the file!\n";
				_exit(-1);
			}
			wr.Run(p, f, &sch);
			wr.WaitUntilDone();
			fclose(f);
		}
	}
}
