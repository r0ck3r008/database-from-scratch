#ifndef QP_TREE_H
#define QP_TREE_H

#include<unordered_map>
#include<vector>

#include"statistics.h"
#include"parser/parse_tree.h"
#include"db/schema.h"

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
	int is_join;
	double cost;
	operation *l_child, *r_child, *parent;

public:
	operation();
	~operation();
};

struct tableInfo
{
	Schema *sch;
	int join_order;
	int sel_order;

public:
	tableInfo();
	~tableInfo();
};

class Qptree
{
	Statistics *s;
	char *catalog_file;
	std :: unordered_map<std :: string, tableInfo>
			relations;
	std :: vector<operation> joins;
	std :: vector<operation> selects;

private:
	void process(struct TableList *);
	void process(struct AndList *);
	void process(struct operation *,
			struct operation *);
	void process(struct AndList *, struct OrList *,
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
