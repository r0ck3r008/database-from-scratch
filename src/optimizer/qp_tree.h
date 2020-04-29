#ifndef QP_TREE_H
#define QP_TREE_H

#include<iostream>
#include<unordered_map>
#include<vector>
#include<queue>

#include"fs/record.h"
#include"db/schema.h"
#include"mem/pipe.h"
#include"statistics.h"
#include"parser/parse_tree.h"
#include"operation.h"
#include"tableinfo.h"
#include"db/query.h"

struct operation;
struct tableInfo;

struct op_comp_join
{
	bool operator()(operation *, operation *);
};

class Qptree
{
	Statistics *s;
	Catalog *c;
	struct operation *tree;
	std::priority_queue<operation *,
		std::vector<operation *>,
		op_comp_join> join_queue;
	std::unordered_map<std::string, tableInfo *>
		relations;
	Pipe *curr_pipe;
	int curr_pipe_id;

private:
	void process(struct TableList *);
	void mk_ops(struct AndList *);
	void process(struct AndList *,
			struct OrList *);
	struct operation *dispense_join(
		struct operation *,
		std::vector<struct operation *> &,
						int);
	void process(struct operation *,
		std::vector<struct operation *> &);
	//grp by
	int process(struct NameList *,
			struct FuncOperator *);
	//aggregate
	void process(struct FuncOperator *);
	//distinct
	void process();
	//project
	void process(struct NameList *);
	Schema *mk_agg_sch();
	void execute(struct query *);

public:
	Qptree(Catalog *);
	~Qptree();
	void process(struct query *);
	Pipe *dispense_pipe(int *);
};

void mk_parent(Qptree *, struct operation *,
			struct operation *, int);

#endif
