#include<string.h>
#include<unistd.h>
#include<errno.h>

#include"tournament.h"
#include"run_merge.h"

thread :: thread(pthread_t *tid, Pipe *pipe, int r_start, int r_size)
{
	this->pipe=pipe;
	this->tid=tid;
	this->r_start=r_start;
	this->r_size=r_size;
}

RunMerge :: RunMerge(Pipe *out_pipe,
			std :: vector <int> *run_sizes,
			struct OrderMaker *order)
{
	this->run_sizes=run_sizes;
	this->out_pipe=out_pipe;
	this->n_runs=this->run_sizes->size();
	this->comp=new struct comparator(NULL, NULL, (void *)order,
						NULL, 0);
	this->tour=new Tournament(this->n_runs, this->comp);
	this->threads=NULL;
}

RunMerge :: ~RunMerge()
{
	for(int i=0; i<this->n_runs; i++) {
		struct thread *arg=this->threads[i];
		delete arg->pipe;
		delete arg;
	}
	delete[] this->threads;
	delete tour;
}

thread *RunMerge :: init_thread(pthread_t *tid, int r_start,
						int r_size)
{
	Pipe *pipe=new Pipe(100);
	struct thread *arg=new struct thread(tid, pipe, r_start, r_size);

	return arg;
}

void RunMerge :: join_wait()
{
	for(int i=0; i<this->n_runs; i++) {
		struct thread *arg=this->threads[i];
		int stat=pthread_join(*(arg->tid), NULL);
/*		if(stat) {
			std :: cerr << "Error in joining thread "
				<< i << ": " << strerror(stat)
				<< std :: endl;
		}*/
	}
}

void RunMerge :: merge_init()
{
	this->threads=new struct thread *[this->run_sizes->size()];

	int cum_size=0;
	for(int i=0; i<this->n_runs; i++) {
		pthread_t tid;
		int curr_size=(*(this->run_sizes))[i];
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
	this->new_feed();
	this->join_wait();
}

int RunMerge :: get_winner(Record **rec)
{
	int pos;
	if((pos=this->tour->get_nxt_spot(0))<0)
		//wait till one is free
		return 1;
	//map to thread indeces
	pos=pos-(this->n_runs-1);
	Record *tmp;
	if(this->threads[pos]->r_size!=0) {
		tmp=new Record;
		if(!this->threads[pos]->pipe->Remove(tmp)) {
			//end of input from first thread
			this->threads[pos]->r_size=0;
			delete tmp;
			tmp=NULL;
		}
	} else {
		tmp=NULL;
	}
	if(this->n_runs<=1) {
		*rec=tmp;
		if(tmp==NULL)
			return 0;
		return 1;
	}
	//feed it
	if(!this->tour->feed(tmp)) {
		if(this->threads[pos]->r_size)
			std :: cerr << "Error in feeding "
				<< pos << std :: endl;
		return 0;
	}
	*rec=NULL;
	if((*rec=this->tour->get_nxt_winner())==NULL)
		return 1;

	return 1;
}

void RunMerge :: new_feed()
{
	Record **tmp;
	while(1) {
		tmp=new Record *;
		if(!this->get_winner(tmp))
			break;
		if(*tmp!=NULL)
			this->out_pipe->Insert(*tmp);
		delete tmp;
	}
	this->out_pipe->ShutDown();
}

void *thread_handler(void *a)
{
	struct thread *arg=(struct thread *)a;
	struct DBFile *dbf=new DBFile;
	if(!dbf->Open("bin/runs.bin")) {
		std :: cerr << "Error in opening DBFile: "
			<< arg->r_start;
		_exit(-1);
	}
	dbf->MoveFirst();

	//reach required record
	int start=arg->r_start, size=arg->r_size;
	Record *tmp=new Record;
	for(int i=0; i<(start+size); i++) {
		if(!dbf->GetNext(tmp)) {
			std :: cerr << "Error in getting record!\n";
			break;
		}
		if(i<arg->r_start)
			delete tmp;
		else
			arg->pipe->Insert(tmp);

		tmp=new Record;
	}

	arg->pipe->ShutDown();
	if(!dbf->Close()) {
		std :: cerr << "Error in closing dbf!\n";
		_exit(-1);
	}
	delete dbf;
	pthread_exit(NULL);
}
