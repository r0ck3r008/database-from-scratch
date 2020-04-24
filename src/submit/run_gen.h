#ifndef RUN_GEN_H
#define RUN_GEN_H

#include"comparison_engine.h"
#include"tournament.h"
#include"record.h"
#include"dbfile.h"
#include"pipe.h"

#include<vector>

class RunGen
{
private:
	Pipe *in_pipe;
	Record *buf;
	DBFile *dbf;
	struct comparator *comp;
	int run_len;
	int size_curr_run;
	char *run_file;

private:
	//functions
	void write_dummy();
	void write(std :: queue <Record *> *);
	int fetch_rec(Record **);

public:
	int setup_dbf();
	std :: vector <int> rec_sizes;
	RunGen(Pipe *, int, OrderMaker *,
		char *);
	~RunGen();
	std :: vector <int> *generator();
};

#endif
