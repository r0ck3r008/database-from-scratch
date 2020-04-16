#ifndef RUN_MERGE_H
#define RUN_MERGE_H

#include<pthread.h>
#include<vector>

#include"pipe.h"
#include"comparator/comparison_engine.h"
#include"fs/record.h"
#include"fs/dbfile.h"

class RunMerge
{
private:
	Tournament *tour;
	struct comparator *comp;
	Pipe *out_pipe;
	struct thread **threads;
	int n_runs;
	std :: vector <int> *run_sizes;
	char *run_file;

private:
	struct thread *init_thread(pthread_t *, int, int);
	void join_wait();
	int get_winner(Record **);
	void new_feed();
public:
	RunMerge(Pipe *, std :: vector <int> *,
			OrderMaker *, char *);
	~RunMerge();
	void merge_init();
};

struct thread
{
	pthread_t *tid;
	Pipe *pipe;
	int r_start;
	int r_size;
	char *run_file;
public:
	thread(pthread_t *, Pipe *, int, int, char *);
};
void *thread_handler(void *);

#endif
