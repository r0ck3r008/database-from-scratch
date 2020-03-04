#ifndef BIGQ_H
#define BIGQ_H

#include<pthread.h>

#include"file.h"
#include"comparison.h"
#include"record.h"
#include"pipe.h"

struct thread_arg
{
	Pipe *in_pipe;
	Pipe *out_pipe;
	int run_len;
	OrderMaker *order;
public:
	thread_arg(Pipe *, Pipe *,
			int, OrderMaker *,
			File *, Page *);
};

class BigQ
{
public:
	BigQ(Pipe *, Pipe *, OrderMaker *, int);
};

void *wrkr_run(void *);
struct thread_arg *wrkr_init(struct thread_arg *);

#endif
