#ifndef QP_TREE_H
#define QP_TREE_H

#include<unordered_map>
#include<queue>
#include<vector>

#include"statistics.h"
#include"parser/parse_tree.h"
#include"db/schema.h"

struct operation;
class Qptree;

struct operation_comp
{
	bool operator()(operation *, operation *);
};

struct tableInfo
{
	Schema *sch;
	int join_order;
	int sel_order;
	std :: priority_queue<operation *,
		std :: vector<operation *>,
		operation_comp> sel_queue;

public:
	tableInfo();
	~tableInfo();
	int dispense_select(struct operation **);
};

enum typeFlags : uint8_t
{
	join_op=1<<1,
	sel_pipe=1<<2,
	sel_file=1<<3,
	sel_any=1<<4,
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
	int type;
	double cost;
	operation *l_child, *r_child, *parent;
	std :: vector<std :: unordered_map<std ::
		string, tableInfo> :: iterator> tables;

public:
	operation();
	operation(struct AndList *, Qptree *);
	~operation();
};

class Qptree
{
	friend struct operation;
	Statistics *s;
	char *catalog_file;
	std :: unordered_map<std :: string, tableInfo>
			relations;
	std :: priority_queue<operation *,
		std :: vector<operation *>,
		operation_comp> join_queue;

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

public:
	Qptree(char *, char *);
	~Qptree();

	void process(struct query *);
};

#endif
