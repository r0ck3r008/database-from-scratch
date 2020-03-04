#ifndef RUN_MERGE_H
#define RUN_MERGE_H

#include<pthread.h>
#include<vector>

#include"pipe.h"
#include"comparison_engine.h"
#include"record.h"
#include"dbfile.h"

class RunMerge
{
private:
	Tournament *tour;
	struct comparator *comp;
	Pipe *out_pipe;
	struct thread **threads;
	int n_runs;
	std :: vector <int> *run_sizes;

private:
	struct thread *init_thread(pthread_t *, int, int);
	void join_wait();
	int get_winner(Record **);
	void new_feed();
public:
	RunMerge(Pipe *, std :: vector <int> *, OrderMaker *);
	~RunMerge();
	void merge_init();
};

struct thread
{
	pthread_t *tid;
	Pipe *pipe;
	int r_start;
	int r_size;
public:
	thread(pthread_t *, Pipe *, int, int);
};
void *thread_handler(void *);

#endif
