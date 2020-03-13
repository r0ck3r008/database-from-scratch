#ifndef REL_OP_H
#define REL_OP_H

#include<pthread.h>

#include"mem/pipe.h"
#include"fs/dbfile.h"
#include"fs/record.h"
#include"function.h"
#include"comparison_engine.h"

struct thr_args
{
public:
	DBFile *dbf;
	Pipe *in_pipe, *in_pipeR, *out_pipe;
	CNF *cnf;
	Record *literal;
	struct comparator *comp;
	FILE *f;
	Schema *sch;
	int *keep, atts_in, atts_out;
};

class RelationalOp
{
public:
	// blocks the caller until the particular relational operator
	// has run to completion
	virtual void WaitUntilDone ();

	// tell us how much internal memory the operation can use
	virtual void Use_n_Pages (int);
};

class SelectFile : public RelationalOp
{
private:
	struct thr_args *arg;
	pthread_t tid;
	int max_pgs;

public:
	SelectFile();
	~SelectFile();
	void Run (DBFile *, Pipe *, CNF *, Record *);
	void WaitUntilDone ();
	void Use_n_Pages (int);

};

class SelectPipe : public RelationalOp
{
private:
	struct thr_args *arg;
	pthread_t tid;
	int max_pgs;

public:
	SelectPipe();
	~SelectPipe();
	void Run (Pipe *, Pipe *, CNF *, Record *);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

class Project : public RelationalOp
{
private:
	struct thr_args *arg;
	pthread_t tid;
	int max_pgs;

public:
	Project();
	~Project();
	void Run (Pipe *, Pipe *, int *, int, int);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

class Join : public RelationalOp
{
private:
	struct thr_args *arg;
	pthread_t tid;
	int max_pgs;

public:
	Join();
	~Join();
	void Run (Pipe *, Pipe *, Pipe *, CNF *, Record *);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

class DuplicateRemoval : public RelationalOp
{
public:
	void Run (Pipe *, Pipe *, Schema *);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

class Sum : public RelationalOp
{
public:
	void Run (Pipe *, Pipe *, Function *);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

class GroupBy : public RelationalOp
{
public:
	void Run (Pipe *, Pipe *, OrderMaker *, Function *);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

class WriteOut : public RelationalOp
{
private:
	struct thr_args *arg;
	pthread_t tid;
	int max_pgs;

public:
	WriteOut();
	~WriteOut();
	void Run (Pipe *, FILE *, Schema *);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

#endif
