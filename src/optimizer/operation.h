#ifndef OPERATION_H
#define OPERATION_H

#include<iostream>

#include"rel_ops/ops.h"
#include"tableinfo.h"
#include"parser/parse_tree.h"

struct tableInfo;

enum type_flag : uint16_t
{
	no_op=0,
	self_f=1<<1,
	selp_f=1<<2,
	join_f=1<<3,
	sum_f=1<<4,
	grpby_f=1<<5,
	proj_f=1<<6,
	dist_f=1<<7,
	wrout_f=1<<8
};

enum pipe_type : uint8_t
{
	left_in=1<<1,
	right_in=1<<2,
	parent_out=1<<3
};

struct operation
{
	type_flag type;
	union
	{
		struct self_op self;
		struct selp_op selp;
		struct join_op join;
		struct grpby_op grpby;
		struct sum_op sum;
		struct proj_op proj;
		struct dist_op dist;
	};
	operation *lchild, *rchild, *parent;
	int lid, rid, pid;
	double cost;
	std::vector<tableInfo *> tables;
	Schema *oschl, *oschr;

	operation(type_flag, double,
		std::vector<tableInfo *> &);
	operation(type_flag, Schema *);
	~operation();
	void traverse(int);
	void add_pipe(pipe_type, Pipe *);
	void append_sch(int, struct operation *);
};

#endif
