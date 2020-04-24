#ifndef TEST_H
#define TEST_H

#include"statistics.h"
#include"parse_tree.h"
#include"qp_tree.h"

// the aggregate function (NULL if no agg)
extern struct FuncOperator *finalFunction;
// the list of tables and aliases in the query
extern struct TableList *tables;
// the predicate in the WHERE clause
extern struct AndList *boolean;
// grouping atts (NULL if no grouping)
extern struct NameList *groupingAtts;
// the set of attributes in the SELECT (NULL if no such atts)
extern struct NameList *attsToSelect;
// 1 if there is a DISTINCT in a non-aggregate query
extern int distinctAtts;
// 1 if there is a DISTINCT in an aggregate query
extern int distinctFunc;

#endif
