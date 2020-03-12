#include<iostream>
#include<string.h>
#include<unistd.h>

#include"../rel_op.h"

SelectPipe :: SelectPipe()
{
	this->arg=new struct thr_args;
}

SelectPipe :: ~SelectPipe()
{
	delete this->arg->comp;
	delete this->arg;
}

void *run_thr(void *a)
{
	char **retval=new char *;
	*retval=new char[256];
	struct thr_args *arg=(struct thr_args *)a;

	Record *tmp=new Record;
	while(1) {
		int stat=arg->in_pipe->Remove(tmp);
		if(!stat)
			break;
		arg->comp->rec1=&tmp;
		if(Compare(arg->comp))
			arg->out_pipe->Insert(tmp);

		delete tmp;
		tmp=new Record;
	}

	arg->out_pipe->ShutDown();
	delete tmp;
	pthread_exit((void **)retval);
}

void SelectPipe :: Run(Pipe *in_pipe, Pipe *out_pipe, CNF *cnf, Record *literal)
{
	struct comparator *comp=new struct comparator(NULL, NULL,
							(void *)literal,
							(void *)cnf, 3);
	this->arg->in_pipe=in_pipe;
	this->arg->out_pipe=out_pipe;
	this->arg->comp=comp;

	int stat=pthread_create(&(this->tid), NULL, run_thr, (void *)this->arg);
	if(!stat) {
		std :: cerr << "Error in creating the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void SelectPipe :: WaitUntilDone()
{
	void **retval;
	int stat=pthread_join(this->tid, retval);

	if(!stat) {
		std :: cerr << "Error in joining the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	} else {
		std :: cout << "Thread successfully exited with: "
			<< *(char **)retval << std :: endl;
	}

	delete[] *(char **)retval;
	delete (char **)retval;
}

void SelectPipe :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}
