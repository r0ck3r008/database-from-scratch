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
	int n_join;
	double cost;
};

struct tableInfo
{
	Schema *sch;
	int join_order;
	int sel_order;
};

class Qptree
{
	Statistics *s;
	char *catalog_file;
	std :: unordered_map<std :: string, tableInfo>
			relations;
	std :: vector<struct operation *> op_list;

private:
	void process(struct TableList *);
	void process(struct AndList *);
	void process(struct operation *,
			struct operation *);
	void process(struct operation *, struct AndList *,
			struct OrList *, char **, int *);
	void get_attr(char *, std :: pair<std :: string,
		std :: unordered_map<std :: string,
		tableInfo> :: iterator> &);

public:
	Qptree(char *, char *);
	~Qptree();

	void process(struct query *);
};

#endif
