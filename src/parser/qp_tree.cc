#include"qp_tree.h"

using namespace std;

query :: query(){}
query :: query(struct FuncOperator *finalFunction, struct TableList *tables,
		struct AndList *boolean, struct NameList *groupingAtts,
		struct NameList *attsToSelect, int distinctAtts,
		int distinctFunc)
{
	this->finalFunction=finalFunction;
	this->tables=tables;
	this->boolean=boolean;
	this->groupingAtts=groupingAtts;
	this->attsToSelect=attsToSelect;
	this->distinctAtts=distinctAtts;
	this->distinctFunc=distinctFunc;
}
query :: ~query(){}

QPTree :: QPTree(){}
QPTree :: QPTree(char *stat_fname)
{
	this->s=new Statistics(stat_fname);
	this->pipe_cnt=0;
}
QPTree :: ~QPTree(){}

void QPTree :: process(struct query *q)
{
	this->process(q->tables);
	this->process(q->boolean);
}

void QPTree :: print()
{

}
