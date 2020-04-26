#ifndef QP_TREE_H
#define QP_TREE_H

#include<iostream>
#include<unordered_map>
#include<vector>
#include<queue>

#include"statistics.h"
#include"parser/parse_tree.h"
#include"operation.h"
#include"db/query.h"

struct op_comp_join
{
	bool operator()(operation *, operation *);
};

struct op_comp_sel
{
	bool operator()(operation *, operation *);
};

class Qptree;
struct tableInfo
{
	Schema *sch;
	std::priority_queue<operation *,
		vector<operation *>, op_comp_sel>
					sel_que;

	void add_sel(struct AndList *, double);
	struct operation *dispense_sel(Qptree *);
};

class Qptree
{
	Statistics *s;
	Catalog *c;
	struct operation *tree;
	std::priority_queue<operation *,
		vector<operation *>, op_comp_join>
					join_queue;
	std::unordered_map<std::string, tableInfo>
		relations;
	Pipe *curr_pipe;
	int curr_pipe_id;

private:
	void process(struct TableList *);
	void mk_ops(struct AndList *);

public:
	Qptree(Catalog *);
	~Qptree();
	void process(struct query *);
	Pipe *dispense_pipe(int *);
};

#endif
