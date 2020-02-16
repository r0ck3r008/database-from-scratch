#ifndef RUN_MERGE_H
#define RUN_MERGE_H

#include<pthread.h>
#include<vector>

#include"pipe.h"
#include"lex/comparison_engine.h"
#include"fs/record.h"
#include"fs/dbfile.h"

class RunMerge
{
private:
	struct thread
	{
		pthread_t *tid;
		Pipe *pipe;
		const RunMerge *ref;
		int r_start;
		int r_size;
	public:
		thread(pthread_t *, Pipe *, const RunMerge *,
			int, int);
		~thread();
	};
	Tournament *tour;
	OrderMaker *order;
	Pipe *out_pipe;
	int n_runs;
	std :: vector <int> *run_sizes;
	struct thread **threads;

private:
	struct thread *init_thread(pthread_t *, int, int);

public:
	RunMerge(Pipe *, std :: vector <int> *, OrderMaker *);
	~RunMerge();
	void merge_init();
};

void *thread_handler(void *);

#endif
