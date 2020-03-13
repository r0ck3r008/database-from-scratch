#ifndef PROJECT_H
#define PROJECT_H

#include"../rel_op.h"

struct project_args
{
public:
	Pipe *in_pipe;
	Pipe *out_pipe;
	int *keep;
	int atts_in;
	int atts_out;
};

class Project : public RelationalOp
{
private:
	struct project_args *arg;
	pthread_t tid;
	int max_pgs;

public:
	Project();
	~Project();
	void Run (Pipe *, Pipe *, int *, int, int);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};


#endif
