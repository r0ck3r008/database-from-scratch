#include"qp_tree.h"

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

operation :: operation()
{
	this->l_child=NULL;
	this->r_child=NULL;
	this->parent=NULL;
	this->type=no_op;
}
operation :: operation(struct AndList *a_list, Qptree *ref)
{
	this->l_pipe=-1; this->r_pipe=-1; this->p_pipe=-1;
	this->a_list=a_list;
	if(a_list->rightAnd!=NULL)
		//assuming all the right Ands have already been dealt with
		a_list->rightAnd=NULL;

	//max 16 relations
	char **rels=new char *[16];
	int indx=0;
	ref->process(this, a_list, NULL, rels, &indx);
	this->cost=ref->s->Estimate(a_list, rels, indx);
	//assuming atomic operations per AndList
	for(int i=0; i<indx; i++) {
		string r_name=string(rels[i]);
		auto itr=ref->relations.find(r_name);
		if(indx==2)
			ref->join_queue.push(this);
		else
			itr->second.sel_queue.push(this);
	}
	for(int i=0; i<indx; i++)
		delete[] rels[i];
	delete[] rels;
}
operation :: ~operation(){}

bool sel_op_comp :: operator()(operation *l, operation *r)
{
	return (l->cost < r->cost);
}

bool join_op_comp :: operator()(operation *l, operation *r)
{
	return (l->cost > r->cost);
}

tableInfo :: tableInfo(){}
tableInfo :: ~tableInfo(){}

int tableInfo :: dispense_select(struct operation **sel_p)
{
	// Make the select file with the greatest cost
	// The join must receive minimun tuples and hence select with least cost
	// is selected as the head of select chain.
	struct operation *sel_op=NULL;
	while(this->sel_queue.size()) {
		if(sel_op==NULL) {
			sel_op=this->sel_queue.top();
			sel_op->type=sel_file;
		} else {
			int pipe=this->qpt->dispense_pipe();
			sel_op->parent=this->sel_queue.top();
			sel_op->parent->l_child=sel_op;
			sel_op->p_pipe=pipe;
			sel_op->parent->l_pipe=pipe;
			sel_op=sel_op->parent;
			sel_op->type=sel_pipe;
		}
		this->sel_queue.pop();
	}

	*sel_p=sel_op;

	if(sel_op==NULL)
		return 0;
	else
		return 1;
}

Qptree :: Qptree(char *stat_fname, char *catalog_file)
{
	this->s=new Statistics;
	this->s->Read(stat_fname);
	this->catalog_file=catalog_file;
	this->pipe_count=0;
}

Qptree :: ~Qptree()
{
	delete this->s;
	for(auto &i: this->relations)
		delete i.second.sch;
}

void Qptree :: process(struct query *q)
{
	this->process(q->tables);
	this->process(q->boolean);
}
