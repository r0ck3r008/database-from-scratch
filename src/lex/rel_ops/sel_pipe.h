#ifndef SEL_PIPE_H
#define SEL_PIPE_H

#include"../rel_op.h"

struct pipe_args
{
public:
	Pipe *in_pipe;
	Pipe *out_pipe;
	struct comparator *comp;
};

class SelectPipe : public RelationalOp
{
private:
	struct pipe_args *arg;
	pthread_t tid;
	int max_pgs;

public:
	SelectPipe();
	~SelectPipe();
	void Run (Pipe *, Pipe *, CNF *, Record *);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

#endif
