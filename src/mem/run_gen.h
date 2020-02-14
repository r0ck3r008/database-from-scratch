#ifndef RUN_GEN_H
#define RUN_GEN_H

#include"lex/comparison_engine.h"
#include"tournament.cc"
#include"fs/record.h"
#include"pipe.h"

class RunGen
{
private:
	Pipe *in_pipe;
	Tournament <Record> *tour;
	int run_len;
	struct comparator *comp;
	OrderMaker *order;


public:
	RunGen(Pipe *, int, OrderMaker *);
	~RunGen();
};

#endif
