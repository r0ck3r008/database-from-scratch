#include<string.h>
#include<unistd.h>
#include<errno.h>

#include"tournament.h"
#include"run_merge.h"

RunMerge :: thread :: thread(pthread_t *tid, Pipe *pipe,
				const RunMerge *ref, int r_no,
				int r_size)
{
	this->pipe=pipe;
	this->tid=tid;
	this->ref=ref;
	this->r_start=r_no;
	this->r_size=r_size;
}

RunMerge :: RunMerge(Pipe *out_pipe,
			std :: vector <int> *run_sizes,
			struct OrderMaker *order)
{
	this->run_sizes=run_sizes;
	this->out_pipe=out_pipe;
	this->tour=new Tournament(this->n_runs, order);
	this->order=order;
	this->threads=NULL;
	this->n_runs=this->run_sizes->size();
}

RunMerge :: ~RunMerge()
{
	delete tour;
	for(int i=0; i<this->n_runs; i++) {
		struct thread *arg=this->threads[i];
		int stat=pthread_join(*(arg->tid), NULL);
		if(stat) {
			std :: cerr << "Error in joining thread: "
				<< i << " " << strerror(stat)
				<< std :: endl;
		}

		delete arg;
	}

	delete[] this->threads;
}

RunMerge :: thread *RunMerge :: init_thread(pthread_t *tid, int r_no,
						int r_size)
{
	Pipe *pipe=new Pipe(100);
	struct thread *arg=new struct thread(tid, pipe, this, r_no,
						r_size);

	return arg;
}

void RunMerge :: merge_init()
{
	this->threads=new struct thread *[this->run_sizes->size()];

	int cum_size=0;
	for(int i=0; i<this->n_runs; i++) {
		pthread_t tid;
		int curr_size=(*(this->run_sizes))[i]+1;
		struct thread *arg=this->init_thread(&tid, cum_size,
							curr_size);
		cum_size+=curr_size;
		int stat=pthread_create(&tid, NULL, thread_handler,
					(void *)arg);
		if(stat) {
			std :: cerr << "Error in creating thread: "
				<< i << " " << strerror(stat)
				<< std :: endl;
			_exit(-1);
		}

		(this->threads)[i]=arg;
	}
}

void *thread_handler(void *a)
{
	struct thread *arg=(struct thread *)a;

	return NULL;
}
