#ifndef QUERY_H
#define QUERY_H

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

	query(struct FuncOperator *,
		struct TableList *,
		struct AndList *,
		struct NameList *,
		struct NameList *,
		int, int);
	~query();
};

void del_name_list(struct NameList *);
void del_tbl_list(struct TableList *);
void del_and_list(struct AndList *,
			struct OrList *);
void del_func_list(struct FuncOperator *);

#endif
