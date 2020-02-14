#ifndef RUN_GEN_H
#define RUN_GEN_H

#include"tournament.cc"
#include"fs/record.h"
#include"pipe.h"

class RunGen
{
private:
	Pipe *in_pipe;
	Tournament <Record> *tour;
	int run_len;

public:
	RunGen(Pipe *, int);
	~RunGen();
};

#endif
