#ifndef REM_DUP_H
#define REM_DUP_H

#include"rel_ops.h"

struct dup_args
{
public:
	Pipe *in_pipe;
	Pipe *out_pipe;
	OrderMaker *order;
};

class DuplicateRemoval
{
private:
	struct dup_args *args;
	int max_pgs;
	pthread_t tid;

public:
	DuplicateRemoval();
	~DuplicateRemoval();
	void Run(Pipe *, Pipe *, Schema *);
	void WaitUntilDone();
	void Use_n_Pages(int);
};

void *dup_thr(void *);

#endif
