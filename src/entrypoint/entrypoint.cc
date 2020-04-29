#include <iostream>

#include"entrypoint.h"

using namespace std;

extern "C" {
	int yyparse(void);   // defined in y.tab.c
}

int main ()
{
	cout << "Enter the query: \n";
	yyparse();

	struct query q(finalFunction, tables, boolean, groupingAtts,
			attsToSelect, distinctAtts, distinctFunc,
			queryType, outputVar, tableName, fileToInsert,
			attsToCreate, attsToSort);
	q.dispatcher();
}
