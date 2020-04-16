#include<string.h>
#include<unistd.h>

#include"rem_dup.h"

void *dup_thr(void *a)
{
	struct dup_args *arg=(struct dup_args *)a;
	Pipe tmp_pipe(100);
	BigQ bq(arg->in_pipe, &tmp_pipe, arg->order, 8);
	struct comparator comp((void *)arg->order, NULL, 0);

	Record tmp1, tmp2;
	tmp_pipe.Remove(&tmp1);
	int stat2=tmp_pipe.Remove(&tmp2);
	int flag;
	while(stat2!=0) {
		flag=0;
		comp.rec1=&tmp1;
		comp.rec2=&tmp2;
		if(Compare(&comp)) {
			arg->out_pipe->Insert(&tmp1);
			tmp1.Consume(&tmp2);
			flag=1;
		}
		explicit_bzero(&tmp2, sizeof(Record));
		stat2=tmp_pipe.Remove(&tmp2);
	}
	if(!flag)
		arg->out_pipe->Insert(&tmp1);

	arg->out_pipe->ShutDown();
	pthread_exit(NULL);
}

DuplicateRemoval :: DuplicateRemoval()
{
	this->args=new struct dup_args;
}

DuplicateRemoval :: ~DuplicateRemoval()
{
	delete this->args->order;
	delete this->args;
}

void DuplicateRemoval :: Run(Pipe *in_pipe, Pipe *out_pipe,
				Schema *sch)
{
	this->args->in_pipe=in_pipe;
	this->args->out_pipe=out_pipe;
	OrderMaker *order=new OrderMaker(sch);
	this->args->order=order;

	int stat=pthread_create(&(this->tid), NULL, dup_thr,
				(void *)this->args);
	if(stat) {
		std :: cerr << "Error in creating the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void DuplicateRemoval :: WaitUntilDone()
{
	int stat=pthread_join(this->tid, NULL);
	if(stat) {
		std :: cerr << "Error in Joining the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void DuplicateRemoval :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}
