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
};

class Qptree
{
	Statistics *s;
	char *catalog_file;
	std :: unordered_map<std :: string, Schema *>
			relations;
	std :: vector<struct operation *> op_list;

private:
	void process(struct TableList *);
	void process(struct AndList *);
	void process(struct OrList *);
	void mk_op_list(struct AndList *);

public:
	Qptree(char *, char *);
	~Qptree();

	void process(struct query *);
};

#endif
