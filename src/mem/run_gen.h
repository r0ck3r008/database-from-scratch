#ifndef RUN_GEN_H
#define RUN_GEN_H

#include"lex/comparison_engine.h"
#include"tournament.cc"
#include"fs/record.h"
#include"fs/dbfile.h"
#include"pipe.h"

#include<vector>

class RunGen
{
private:
	Pipe *in_pipe;
	struct comparator *comp;
	Record **buf;
	DBFile *dbf;
	int run_len;
	int size_curr_run;

private:
	//functions
	int setup_dbf();
	void write_dummy();
	void write(std :: queue <Record *>);
	int fetch_rec(Record **);

public:
	RunGen(Pipe *, int, OrderMaker *);
	~RunGen();
	void generator();
};

#endif
