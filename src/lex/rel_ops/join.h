#ifndef JOIN_H
#define JOIN_H

#include"../rel_op.h"

struct join_args
{
public:
	Pipe *in_pipe;
	Pipe *in_pipeR;
	Pipe *out_pipe;
	CNF *cnf;
	Record *literal;
	OrderMaker *o_left;
	OrderMaker *o_right;
};

class Join
{
private:
	struct join_args *arg;
	pthread_t tid;
	int max_pgs;

public:
	Join();
	~Join();
	void Run (Pipe *, Pipe *, Pipe *,
			CNF *, Record *);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

#endif
