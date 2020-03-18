#include<string.h>
#include<unistd.h>

#include"sel_pipe.h"

void *pipe_thr(void *a)
{
	struct pipe_args *arg=(struct pipe_args *)a;

	Record tmp;
	while(1) {
		int stat=arg->in_pipe->Remove(&tmp);
		if(!stat)
			break;
		arg->comp->rec1=&tmp;
		if(Compare(arg->comp))
			arg->out_pipe->Insert(&tmp);

		explicit_bzero(&tmp, sizeof(Record));
	}

	arg->out_pipe->ShutDown();
	pthread_exit(NULL);
}

SelectPipe :: SelectPipe()
{
	this->arg=new struct pipe_args;
}

SelectPipe :: ~SelectPipe()
{
	delete this->arg->comp;
	delete this->arg;
}

void SelectPipe :: Run(Pipe *in_pipe, Pipe *out_pipe, CNF *cnf, Record *literal)
{
	struct comparator *comp=new struct comparator((void *)literal,
							(void *)cnf, 3);
	this->arg->in_pipe=in_pipe;
	this->arg->out_pipe=out_pipe;
	this->arg->comp=comp;

	int stat=pthread_create(&(this->tid), NULL, pipe_thr,
				(void *)this->arg);
	if(stat) {
		std :: cerr << "Error in creating the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void SelectPipe :: WaitUntilDone()
{
	int stat=pthread_join(this->tid, NULL);

	if(stat) {
		std :: cerr << "Error in joining the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	} else {
		std :: cout << "Thread successfully exited with\n";
	}
}

void SelectPipe :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}
