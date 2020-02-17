#include<string.h>
#include<unistd.h>
#include<errno.h>

#include"tournament.h"
#include"run_merge.h"

thread :: thread(pthread_t *tid, Pipe *pipe,
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
	for(int i=0; i<this->n_runs; i++) {
		struct thread *arg=this->threads[i];
		delete arg->pipe;
		delete arg;
	}
	delete tour;
	delete[] this->threads;
}

thread *RunMerge :: init_thread(pthread_t *tid, int r_no,
						int r_size)
{
	Pipe *pipe=new Pipe(100);
	struct thread *arg=new struct thread(tid, pipe, this, r_no,
						r_size);

	return arg;
}

void RunMerge :: join_wait()
{
	for(int i=0; i<this->n_runs; i++) {
		struct thread *arg=this->threads[i];
		int stat=pthread_join(*(arg->tid), NULL);
		if(stat) {
			std :: cerr << "Error in joining thread: "
				<< i << " " << strerror(stat)
				<< std :: endl;
		}
	}
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
	this->new_feed();
	this->join_wait();
}

void RunMerge :: new_feed()
{
	int size=this->tour->get_player_num();
	int n_players=(size%2==0) ? (size/2) : ((size+1)/2);
	int flag=0;
	while(!flag) {
		int pos;
		if((pos=tour->get_nxt_spot(0))<0)
			//wait till one is free
			continue;
		//map to thread indeces
		pos=pos-(n_players-1);
		Record *tmp;
		if(this->threads[pos]->r_size!=0) {
			tmp=new Record;
			if(!this->threads[pos]->pipe->Remove(tmp))
				//end of input from first thread
				this->threads[pos]->r_size=0;
		} else {
			tmp=NULL;
		}
		//feed it
		if(!this->tour->feed(tmp)) {
			std :: cerr << "Error in feeding "
				<< pos << std :: endl;
			flag=-1;
		}
		tmp=NULL;
		if((tmp=this->tour->get_nxt_winner())==NULL)
			continue;
		this->out_pipe->Insert(tmp);
	}

}

void *thread_handler(void *a)
{
	struct thread *arg=(struct thread *)a;
	struct DBFile *dbf=new DBFile;
	if(!dbf->Open("out/runs.bin")) {
		std :: cerr << "Error in opening DBFile: "
			<< arg->r_start;
		_exit(-1);
	}
	dbf->MoveFirst();

	//reach required record
	Record *tmp=new Record;
	for(int i=0; i<((arg->r_start)+(arg->r_size)-1); i++) {
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
	return NULL;
}