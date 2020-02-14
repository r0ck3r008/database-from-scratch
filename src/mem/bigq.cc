#include<string.h>
#include<errno.h>
#include<unistd.h>

#include"bigq.h"

thread_arg :: thread_arg(Pipe *in_pipe, Pipe *out_pipe,
				int run_len, OrderMaker *order,
				File *f, Page *pg)
{
	this->in_pipe=in_pipe;
	this->out_pipe=out_pipe;
	this->order=order;
	this->run_len=run_len;
	this->f=f;
	this->pg=pg;
}

thread_arg :: ~thread_arg()
{
	if(this->pg!=NULL)
		delete this->pg;
	if(this->f!=NULL)
		delete this->f;
}

BigQ :: BigQ(Pipe *in_pipe, Pipe *out_pipe,
		OrderMaker *order, int run_len)
{
	struct thread_arg *arg=new struct thread_arg(in_pipe, out_pipe,
							run_len, order,
							NULL, NULL);
	pthread_t wrkr_tid;
	int stat=pthread_create(&wrkr_tid, NULL, wrkr_run, (void *)arg);
	if(stat) {
		std :: cerr << "Error in creating wrkr thread of BigQ: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

struct thread_arg *wrkr_init(struct thread_arg *a)
{
	File *f=new File;
	Page *pg=new Page;
	struct thread_arg *arg=new struct thread_arg(a->in_pipe, a->out_pipe,
						a->run_len, a->order, f, pg);
	return arg;
}

void *wrkr_run(void *a)
{
	//instantiate
	struct thread_arg *arg=wrkr_init((struct thread_arg *)a);
exit:
	delete arg;
	return NULL;
}
