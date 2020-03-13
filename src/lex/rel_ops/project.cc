#include<string.h>
#include<unistd.h>

#include"../rel_op.h"

void *run_thr(void *a)
{
	struct thr_args *arg=(struct thr_args *)a;

	Record *tmp=new Record;
	while(1) {
		int stat=arg->in_pipe->Remove(tmp);
		if(!stat)
			break;
		tmp->Project(arg->keep, arg->atts_out, arg->atts_in);
		arg->out_pipe->Insert(tmp);

		delete tmp;
		tmp=new Record;
	}

	delete tmp;
	pthread_exit(NULL);
}

Project :: Project()
{
	this->arg=new struct thr_args;
}

Project :: ~Project()
{
	delete this->arg;
}

void Project :: Run(Pipe *in_pipe, Pipe *out_pipe,
			int *keep, int atts_in, int atts_out)
{
	this->arg->in_pipe=in_pipe;
	this->arg->out_pipe=out_pipe;
	this->arg->keep=keep;
	this->arg->atts_in=atts_in;
	this->arg->atts_out=atts_out;

	int stat=pthread_create(&(this->tid), NULL, run_thr, (void *)this->arg);
	if(!stat) {
		std :: cerr << "Error in cerating the thread: "
			<< strerror(errno) << std :: endl;
		_exit(-1);
	}
}

void Project :: WaitUntilDone()
{
	int stat=pthread_join(this->tid, NULL);

	if(!stat) {
		std :: cerr << "Error in joining the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	} else {
		std :: cout << "Exited successfully\n";
	}
}

void Project :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}
