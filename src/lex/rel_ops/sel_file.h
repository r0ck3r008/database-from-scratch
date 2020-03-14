#ifndef SEL_FILE_H
#define SEL_FILE_H

#include"../rel_op.h"

struct file_args
{
public:
	DBFile *dbf;
	Pipe *out_pipe;
	CNF *cnf;
	Record *literal;
};

class SelectFile : public RelationalOp
{
private:
	struct file_args *arg;
	pthread_t tid;
	int max_pgs;

public:
	SelectFile();
	~SelectFile();
	void Run (DBFile *, Pipe *, CNF *, Record *);
	void WaitUntilDone ();
	void Use_n_Pages (int);

};

void *run_thr(void *);

#endif
