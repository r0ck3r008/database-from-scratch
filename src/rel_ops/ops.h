#ifndef OPS_H
#define OPS_H

#include"fs/dbfile.h"
#include"fs/record.h"
#include"comparator/comparison_engine.h"
#include"parser/parse_tree.h"
#include"mem/pipe.h"
#include"rel_ops.h"
#include"db/schema.h"

struct self_op
{
	SelectFile *self;
	struct AndList *alist;
	Pipe *opipe;
	Schema *sch;

	CNF *cnf;
	Record *literal;

	self_op();
	~self_op();
	void traverse(int, struct operation *);
};

struct selp_op
{
	SelectPipe *selp;
	struct AndList *alist;
	Pipe *ipipe, *opipe;
	Schema *sch;

	CNF *cnf;
	Record *literal;

	selp_op();
	~selp_op();
	void traverse(int, struct operation *);
};

struct join_op
{
	Join *j;
	struct AndList *alist;
	Pipe *ipipe1, *ipipe2, *opipe;

	Schema *schl, *schr;
	Record *literal;
	CNF *cnf;

	join_op();
	~join_op();
	void traverse(int, struct operation *);
};

struct grpby_op
{
	GroupBy *grp;
	OrderMaker *order;
	Function *f;

	FuncOperator *flist;
	Pipe *ipipe, *opipe;
	Schema *sch;

	grpby_op();
	~grpby_op();
	void traverse(int, struct operation *);
};

struct sum_op
{
	Sum *s;
	Function *f;
	Schema *sch;

	FuncOperator *flist;
	Pipe *ipipe, *opipe;

	sum_op();
	~sum_op();
	void traverse(int, struct operation *);
};

struct proj_op
{

};

#endif
