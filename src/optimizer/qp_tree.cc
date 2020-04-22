#include<unistd.h>

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
operation :: operation(int flag)
{
	this->l_child=NULL;
	this->r_child=NULL;
	this->parent=NULL;
	this->type=flag;
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
	if(indx==2) {
		ref->join_queue.push(this);
	} else {
		string r_name=string(rels[0]);
		auto itr=ref->relations.find(r_name);
		itr->second.sel_queue.push(this);
		itr->second.sel_flag=1;
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

struct operation *tableInfo :: dispense_select()
{
	// Make the select file with the greatest cost
	// The join must receive minimun tuples and hence select with least cost
	// is selected as the head of select chain.
	struct operation *sel_op=NULL;
	while(!this->sel_queue.empty()) {
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
	if(sel_op==NULL && !this->sel_flag) {
		//create a dummy operation which is select pipe if no pipes were
		//there to begin with
		sel_op=new operation(sel_pipe);
		sel_flag=1;
	}

	return sel_op;
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
	vector<operation *> j_done;
	stack<operation *> tree_stk;

	while(!this->join_queue.empty()) {
		struct operation *j_op=this->join_queue.top();
		this->join_queue.pop();
		this->process_join(j_op, j_done, tree_stk);
	}

	if(!tree_stk.size()) {
		// in case of no join
		struct operation *op;
		for(auto &i: this->relations) {
			if((op=i.second.dispense_select())==NULL) {
				cerr << "Error in making tree!";
				_exit(-1);
			}
			tree_stk.push(op);
		}
	}
	this->tree=tree_stk.top();
}
