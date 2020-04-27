#ifndef OPERATION_H
#define OPERATION_H

#include<iostream>

#include"tableinfo.h"
#include"rel_ops/ops.h"
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
	};
	operation *lchild, *rchild, *parent;
	int lid, rid, pid;
	double cost;
	std::vector<tableInfo *> tables;
	std::vector<tableInfo *> rels;

	operation(type_flag);
	~operation();
	void exec();
	void print();
};

#endif
