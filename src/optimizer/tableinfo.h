#ifndef TABLEINFO_H
#define TABLEINFO_H

#include<iostream>
#include<vector>
#include<queue>

#include"db/schema.h"
#include"parser/parse_tree.h"
#include"operation.h"
#include"qp_tree.h"

class Qptree;
struct operation;

struct op_comp_sel
{
	bool operator()(operation *, operation *);
};

struct tableInfo
{
	Schema *sch;
	std::priority_queue<operation *,
		std::vector<operation *>,
		op_comp_sel> sel_que;

	void add_sel(struct AndList *, double);
	struct operation *dispense_sel(Qptree *);
};


#endif
