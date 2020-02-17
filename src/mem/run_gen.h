#ifndef RUN_GEN_H
#define RUN_GEN_H

#include"lex/comparison_engine.h"
#include"tournament.h"
#include"fs/record.h"
#include"fs/dbfile.h"
#include"pipe.h"

#include<vector>

class RunGen
{
private:
	Pipe *in_pipe;
	Record *buf;
	DBFile *dbf;
	struct OrderMaker *order;
	int run_len;
	int size_curr_run;

private:
	//functions
	void write_dummy();
	void write(std :: queue <Record *> *);
	int fetch_rec(Record **);

public:
	int setup_dbf();
	std :: vector <int> rec_sizes;
	RunGen(Pipe *, int, OrderMaker *);
	~RunGen();
	std :: vector <int> *generator();
};

#endif
