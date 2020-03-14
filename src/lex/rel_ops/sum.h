#ifndef SUM_H
#define SUM_H

#include"../rel_op.h"

struct sum_args
{
public:
	Pipe *in_pipe;
	Pipe *out_pipe;
	Function *func;
};

class Sum : public RelationalOp
{
private:
	struct sum_args *args;
	pthread_t tid;
	int max_pgs;

public:
	Sum();
	~Sum();
	void Run(Pipe *, Pipe *, Function *);
	void WaitUntilDone();
	void Use_n_Pages(int);
};

void *run_thr(void *);

#endif
