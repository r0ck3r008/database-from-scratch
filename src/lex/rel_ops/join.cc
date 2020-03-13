#include<string.h>
#include<unistd.h>

#include"join.h"

void *run_thr(void *a)
{
	struct join_args *arg=(struct join_args *)a;
	char **retval=new char *;
	*retval=new char[256];

	Record *tmp1=new Record;
	Record *tmp2=new Record;
	while(1) {
		int stat1=arg->in_pipe->Remove(tmp1);
		int stat2=arg->in_pipeR->Remove(tmp2);
		if(!stat1 && !stat2)
			break;
		arg->comp->rec1=&tmp1;
		arg->comp->rec2=&tmp2;
	}

	pthread_exit((void **)retval);
}

Join :: Join()
{
	this->arg=new struct join_args;
}

Join :: ~Join()
{
	delete this->arg->comp;
	delete this->arg;
}

void Join :: Run(Pipe *in_pipeL, Pipe *in_pipeR, Pipe *out_pipe,
			CNF *cnf, Record *literal)
{
	this->arg->in_pipe=in_pipeL;
	this->arg->in_pipeR=in_pipeR;
	this->arg->out_pipe=out_pipe;
	struct comparator *comp=new struct comparator(NULL, NULL,
						(void *)literal,
						(void *)cnf, 2);

	int stat=pthread_create(&(this->tid), NULL, run_thr, (void *)this->arg);
	if(!stat) {
		std :: cerr << "Error in creating the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void Join :: WaitUntilDone()
{
	void **retval;
	int stat=pthread_join(this->tid, retval);

	if(!stat) {
		std :: cerr << "Error in joining to the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	} else {
		std :: cout << "Successful thread exit: "
			<< *(char **)retval << std :: endl;
	}

	delete[] *(char **)retval;
	delete *(char **)retval;
}

void Join :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}
