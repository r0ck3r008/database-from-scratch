#ifndef QP_TREE_H
#define QP_TREE_H

#include<unordered_map>
#include<queue>
#include<vector>
#include<stack>

#include"statistics.h"
#include"parse_tree.h"
#include"schema.h"
#include"comparison.h"
#include"function.h"
#include"record.h"

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

enum typeFlags : uint16_t
{
	join_op=1<<1,
	sel_pipe=1<<2,
	sel_file=1<<3,
	sel_any=1<<4,
	grp_by=1<<5,
	sum=1<<6,
	distinct=1<<7,
	project=1<<8,
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
	struct NameList *att_list;
	OrderMaker *order;
	Schema *grp_sch;
	Schema *agg_sch;
	int type;
	double cost;
	operation *l_child, *r_child, *parent;
	int l_pipe, r_pipe, p_pipe;
	std :: vector<std :: unordered_map<std ::
		string, tableInfo> :: iterator> tables;
	std :: vector<std :: unordered_map<std ::
		string, tableInfo> :: iterator> curr_sch;

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
	void process(int, int);
	void process(struct FuncOperator *);
	void process(struct NameList *, struct NameList *,
			struct FuncOperator *);
	void process(struct NameList *);
	void process(struct NameList *,
			struct FuncOperator *);
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
	struct Schema *mk_sch(struct NameList *,
				  struct operation *);
	void add_distinct();

public:
	Qptree(char *, char *);
	~Qptree();

	void process(struct query *);
	int dispense_pipe();
};

void print_in_order(struct operation *);
void mk_parent(Qptree *, struct operation *,
		struct operation *, int);
void print_f_list(struct FuncOperator *, int);
void print_name_list(struct NameList *);

#endif
