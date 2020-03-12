#include<string.h>
#include<unistd.h>

#include"../rel_op.h"

void *run_thr(void *a)
{
	struct thr_args *arg=(struct thr_args *)a;
	char **retval=new char *;
	*retval=new char[256];

	Record *tmp=new Record;
	while(1) {
		int stat=arg->in_pipe->Remove(tmp);
		if(!stat)
			break;
		if(!tmp->check_null()) {
			char *buf=tmp->deserialize(arg->sch);
			fprintf(arg->f, "%s", buf);
			delete[] buf;
		}
		delete tmp;
		tmp=new Record;
	}

	delete tmp;
	pthread_exit((void **)retval);
}

WriteOut :: WriteOut()
{
	this->arg=new struct thr_args;
}

WriteOut :: ~WriteOut()
{
	delete this->arg;
}

void WriteOut :: Run(Pipe *in_pipe, FILE *f, Schema *sch)
{
	this->arg->in_pipe=in_pipe;
	this->arg->f=f;
	this->arg->sch=sch;

	int stat=pthread_create(&(this->tid), NULL, run_thr, NULL);
	if(!stat) {
		std :: cerr << "Error in starting the thread "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void WriteOut :: WaitUntilDone()
{
	void **retval;
	int stat=pthread_join(this->tid, retval);

	if(!stat) {
		std :: cerr << "Error in joining the new thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	} else {
		std :: cout << "Thread Exited successfully: "
			<< *(char **)retval;
	}

	delete[] *(char **)retval;
	delete (char **)retval;
}

void WriteOut :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}
