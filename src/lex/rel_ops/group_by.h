#ifndef GROUP_BY
#define GROUP_BY

#include<vector>

#include"../rel_op.h"

struct grp_args
{
public:
	Pipe *in_pipe;
	Pipe *out_pipe;
	OrderMaker *order;
	Function *func;
	Sum *s;
};

class GroupBy
{
private:
	struct grp_args *args;
	pthread_t tid;
	int max_pgs;

public:
	GroupBy();
	~GroupBy();
	void Run(Pipe *, Pipe *, OrderMaker *,
			Function *);
	void WaitUntilDone();
	void Use_n_Pages(int);
};

void *grp_thr(void *);
void mk_grps(struct grp_args *,
		vector<Record *> &,
		vector<struct grp_args *> &);
void push_results(struct grp_args *,
		vector<struct grp_args *> &);
#endif
