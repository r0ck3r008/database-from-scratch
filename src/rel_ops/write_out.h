#ifndef WRITE_OUT_H
#define WRITE_OUT_H

#include"rel_op.h"

struct write_args
{
public:
	Pipe *in_pipe;
	FILE *f;
	Schema *sch;
};

class WriteOut
{
private:
	struct write_args *arg;
	pthread_t tid;
	int max_pgs;

public:
	WriteOut();
	~WriteOut();
	void Run (Pipe *, FILE *, Schema *);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

#endif
