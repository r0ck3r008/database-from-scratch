#ifndef QUERY_H
#define QUERY_H

#include"catalog.h"
#include"optimizer/qp_tree.h"
#include"parser/parse_tree.h"

struct query
{
	struct FuncOperator *func;
	struct TableList *tbls;
	struct AndList *a_list;
	struct NameList *grp_atts;
	struct NameList *sel_atts;
	int dis_att;
	int dis_func;
	int query_type;
	char *output_var;
	char *table;
	char *file;
	struct AttrList *atts_create;
	struct NameList *atts_sort;
	query(struct FuncOperator *,
		struct TableList *,
		struct AndList *,
		struct NameList *,
		struct NameList *,
		int, int, int,
		char *, char *, char *,
		struct AttrList *,
		struct NameList *);
	~query();

	void dispatcher();
};

#endif
