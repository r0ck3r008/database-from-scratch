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
	CNF *cnf;
	Record *literal;

	self_op(struct AndList *, Pipe *);
	~self_op();
	void exec();
	void print();
};

struct selp_op
{
	SelectPipe *selp;
	struct AndList *alist;
	Pipe *opipe, *ipipe;

	CNF *cnf;
	Record *literal;

	selp_op(struct AndList *, Pipe *, Pipe *);
	~selp_op();
	void exec();
	void print();
};

struct join_op
{
	Join *j;
	struct AndList *alist;
	Pipe *ipipe1, *ipipe2, *opipe;

	Schema *schl, *schr;
	Record *literal;
	CNF *cnf;

	join_op(struct AndList *, Pipe *, Pipe *, Pipe *);
	~join_op();
	void exec();
	void print();
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
