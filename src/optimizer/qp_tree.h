#ifndef QP_TREE_H
#define QP_TREE_H

#include<unordered_map>
#include<queue>
#include<vector>
#include<stack>

#include"statistics.h"
#include"parser/parse_tree.h"
#include"db/schema.h"
#include"comparator/comparison.h"
#include"fs/record.h"

struct operation;
class Qptree;

struct sel_op_comp
{
	//comparison operation structure for selection
	//operation priority queue used in tableInfo
	bool operator()(operation *, operation *);
};

struct join_op_comp
{
	//comparison operator structure for the join
	//operation priority queue used in Qptree
	bool operator()(operation *, operation *);
};

struct tableInfo
{
	Schema *sch;
	Qptree *qpt;
	int sel_flag;
	std :: priority_queue<operation *,
		std :: vector<operation *>,
		sel_op_comp> sel_queue;

public:
	tableInfo();
	~tableInfo();
	struct operation *dispense_select();
};

enum typeFlags : uint8_t
{
	join_op=1<<1,
	sel_pipe=1<<2,
	sel_file=1<<3,
	sel_any=1<<4,
	grp_by=1<<5,
	sum=1<<6,
	distinct=1<<7,
	no_op=0
};

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
	query(struct FuncOperator *,
		struct TableList *, struct AndList *,
		struct NameList *, struct NameList *,
		int, int);
	~query();
};

struct operation
{
	struct AndList *a_list;
	struct FuncOperator *f_list;
	int type;
	double cost;
	operation *l_child, *r_child, *parent;
	int l_pipe, r_pipe, p_pipe;
	std :: vector<std :: unordered_map<std ::
		string, tableInfo> :: iterator> tables;

public:
	operation();
	operation(struct AndList *, Qptree *);
	operation(int);
	~operation();
	void print();
};

class Qptree
{
	friend struct operation;

	struct operation *tree;
	Statistics *s;
	char *catalog_file;
	int pipe_count;
	std :: unordered_map<std :: string, tableInfo>
			relations;
	std :: priority_queue<operation *,
		std :: vector<operation *>,
		join_op_comp> join_queue;

private:
	void process(struct TableList *);
	void process(struct AndList *);
	void process(struct operation *,
			struct operation *);
	void process(struct operation *,
		struct AndList *, struct OrList *,
				char **, int *);
	void get_attr(char *, std :: pair<std :: string,
		std :: unordered_map<std :: string,
		tableInfo> :: iterator> &);
	void process(struct operation *,
			std :: vector<operation *> &,
			std :: stack<operation *> &);
	struct operation *dispense_join(
			struct operation *, int,
			std :: vector<operation *> &,
			std :: stack<operation *> &);

public:
	Qptree(char *, char *);
	~Qptree();

	void process(struct query *);
	int dispense_pipe();
};

void print_in_order(struct operation *);
void mk_parent(Qptree *, struct operation *,
		struct operation *, int);

#endif
