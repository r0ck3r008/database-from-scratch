#ifndef SUM_H
#define SUM_H

#include"rel_ops.h"

struct sum_args
{
public:
	Pipe *in_pipe;
	Pipe *out_pipe;
	Function *func;
};

class Sum
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

#endif
