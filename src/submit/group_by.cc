#include<string.h>
#include<unistd.h>

#include"group_by.h"

//This functions receives vectors in form of pointers pointing to constant
//memory location so that vectors are not copies across functions.
void mk_grps(struct grp_args *arg, vector<Record *> &grps,
		vector<struct grp_args *> &args)
{
	struct comparator comp((void *)arg->order, NULL, 0);
	Record *tmp=new Record;
	while(1) {
		int stat=arg->in_pipe->Remove(tmp);
		int flag=0;
		if(!stat)
			break;
		for(int i=0; i<grps.size(); i++) {
			comp.rec1=grps[i];
			comp.rec2=tmp;
			if(!Compare(&comp)) {
				flag=1;
				args[i]->in_pipe->Insert(tmp);
				delete tmp;
				break;
			}
		}
		if(!flag) {
			struct grp_args *argument=new struct grp_args;
			argument->in_pipe=new Pipe(100);
			argument->out_pipe=new Pipe(100);
			argument->order=arg->order;
			argument->func=NULL;
			argument->s=new Sum;
			argument->s->Run(argument->in_pipe, argument->out_pipe,
						arg->func);
			args.push_back(argument);
			grps.push_back(tmp);
		}
		tmp=new Record;
	}
	delete tmp;

	for(int i=0; i<args.size(); i++) {
		args[i]->in_pipe->Insert(grps[i]);
		delete grps[i];
		args[i]->in_pipe->ShutDown();
	}
}

void push_results(struct grp_args *arg, vector<struct grp_args *> &args)
{
	for(int i=0; i<args.size(); i++) {
		struct grp_args *argument=args[i];
		argument->s->WaitUntilDone();
		Record tmp;
		argument->out_pipe->Remove(&tmp);
		arg->out_pipe->Insert(&tmp);
		delete argument->in_pipe;
		delete argument->out_pipe;
		delete argument->s;
	}

	arg->out_pipe->ShutDown();
}

void *grp_thr(void *a)
{
	struct grp_args *arg=(struct grp_args *)a;

	vector<Record *> grps;
	vector<struct grp_args *> args;
	mk_grps(arg, grps, args);
	push_results(arg, args);

	pthread_exit(NULL);
}

GroupBy :: GroupBy()
{
	this->args=new struct grp_args;
}

GroupBy :: ~GroupBy()
{
	delete this->args;
}

void GroupBy :: Run(Pipe *in_pipe, Pipe *out_pipe, OrderMaker *order,
			Function *func)
{
	this->args->in_pipe=in_pipe;
	this->args->out_pipe=out_pipe;
	this->args->order=order;
	this->args->func=func;
	this->args->s=NULL;

	int stat=pthread_create(&(this->tid), NULL, grp_thr,
				(void *)this->args);
	if(stat) {
		std :: cerr << "Error in starting the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void GroupBy :: WaitUntilDone()
{
	int stat=pthread_join(this->tid, NULL);
	if(stat) {
		std :: cerr << "Error in joining to the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void GroupBy :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}
