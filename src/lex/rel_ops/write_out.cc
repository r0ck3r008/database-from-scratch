#include<string.h>
#include<unistd.h>

#include"write_out.h"

void *write_thr(void *a)
{
	struct write_args *arg=(struct write_args *)a;

	Record tmp;
	while(1) {
		int stat=arg->in_pipe->Remove(&tmp);
		if(!stat)
			break;
		if(!tmp.check_null()) {
			char *buf=tmp.deserialize(arg->sch);
			fprintf(arg->f, "%s", buf);
			delete[] buf;
		}
		explicit_bzero(&tmp, sizeof(Record));
	}

	pthread_exit(NULL);
}

WriteOut :: WriteOut()
{
	this->arg=new struct write_args;
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

	int stat=pthread_create(&(this->tid), NULL, write_thr,
				(void *)this->arg);
	if(stat) {
		std :: cerr << "Error in starting the thread "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void WriteOut :: WaitUntilDone()
{
	int stat=pthread_join(this->tid, NULL);

	if(stat) {
		std :: cerr << "Error in joining the new thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	} else {
		std :: cout << "Thread Exited successfully\n";
	}
}

void WriteOut :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}
