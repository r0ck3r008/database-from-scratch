#ifndef QP_TREE_H
#define QP_TREE_H

#include<queue>

#include"optimizer/statistics.h"
#include"parse_tree.h"
#include"mem/qp_node.h"

struct query
{
	struct FuncOperator *finalFunction;
	struct TableList *tables;
	struct AndList *boolean;
	struct NameList *groupingAtts;
	struct NameList *attsToSelect;
	int distinctAtts;
	int distinctFunc;

public:
	query();
	query(struct FuncOperator *,
		struct TableList *, struct AndList *,
		struct NameList *, struct NameList *,
		int, int);
	~query();
};

class QPTree
{
	Node *tree;
	Statistics *s;
	struct query *q;
	int pipe_cnt;

public:
	QPTree();
	QPTree(struct query *, char *);
	~QPTree();
	void process();
	void print();
};

#endif
