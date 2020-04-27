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
	void traverse(int);
};

struct selp_op
{
	SelectPipe *selp;
	struct AndList *alist;
	Pipe *ipipe, *opipe;

	CNF *cnf;
	Record *literal;

	selp_op();
	~selp_op();
	void traverse(int);
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
	void traverse(int);
};

struct grpby_op
{

};

struct sum_op
{

};

struct proj_op
{

};

#endif
