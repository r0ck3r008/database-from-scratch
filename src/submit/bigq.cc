#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<vector>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>

#include"bigq.h"
#include"run_gen.h"
#include"run_merge.h"

thread_arg :: thread_arg(Pipe *in_pipe, Pipe *out_pipe,
				int run_len, OrderMaker *order,
				File *f, Page *pg)
{
	this->in_pipe=in_pipe;
	this->out_pipe=out_pipe;
	this->order=order;
	this->run_len=run_len;
	long int rand_num=random();
	this->run_file=new char[128];
	sprintf(this->run_file, "runs_%ld.bin", rand_num);
}

BigQ :: BigQ(Pipe *in_pipe, Pipe *out_pipe,
		OrderMaker *order, int run_len)
{
	struct thread_arg *arg=new struct thread_arg(in_pipe, out_pipe,
							run_len, order,
							NULL, NULL);

	int stat=pthread_create(&(this->tid), NULL, wrkr_run, (void *)arg);
	if(stat) {
		std :: cerr << "Error in creating wrkr thread of BigQ: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

BigQ :: ~BigQ()
{
	int stat=pthread_join(this->tid, NULL);
	if(stat) {
		std :: cerr << "Error in joining to wrkr thread of bigq: "
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

	RunGen run_gen(arg->in_pipe, arg->run_len, arg->order, arg->run_file);
	std :: vector <int> *rec_sizes=run_gen.generator();
	RunMerge run_merge(arg->out_pipe, rec_sizes, arg->order, arg->run_file);
	run_merge.merge_init();
exit:
	struct stat buf;
	if(stat(arg->run_file, &buf)==0) {
		if(unlink(arg->run_file)<0) {
			std :: cerr << "Error in deleting "
				<< arg->run_file << " file: "
				<< strerror(errno);
			_exit(-1);
		}
	}
	delete[] arg->run_file;
	delete arg;
	pthread_exit(NULL);
}
