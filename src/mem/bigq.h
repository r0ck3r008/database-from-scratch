#ifndef BIGQ_H
#define BIGQ_H

#include<pthread.h>

#include"fs/file.h"
#include"lex/comparison.h"
#include"fs/record.h"
#include"pipe.h"
#include"tournament.h"

struct thread_arg
{
	Pipe *in_pipe;
	Pipe *out_pipe;
	int run_len;
	OrderMaker *order;
	File *f;
	Page *pg;
	Tournament <Record> *tour;

public:
	thread_arg(Pipe *, Pipe *,
			int, OrderMaker *,
			File *, Page *,
			Tournament <Record> *);
	~thread_arg();
};

class BigQ
{
public:
	BigQ(Pipe *, Pipe *, OrderMaker *, int);
	~BigQ();
};

void *wrkr_run(void *);
struct thread_arg *wrkr_init(struct thread_arg *);

#endif
