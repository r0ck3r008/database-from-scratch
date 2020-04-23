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
	this->a_list=NULL;
	this->type=no_op;
}
operation :: operation(int flag)
{
	this->l_child=NULL;
	this->r_child=NULL;
	this->parent=NULL;
	this->a_list=NULL;
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

void operation :: print()
{
	CNF cnf;
	Record literal;
	cout << "**********\n";
	if(this->type & join_op) {
		cout << "JOIN OPERATION:\n";
		cout << "CNF: \n";
		Schema *sch_l=this->tables[0]->second.sch;
		Schema *sch_r=this->tables[1]->second.sch;
		cnf.GrowFromParseTree(this->a_list, sch_l, sch_r, literal);
		cnf.Print();
		cout << "Output pipe ID: " << this->p_pipe << endl;
		cout << "Input pipe ID: ";
		cout << this->l_pipe << ", " << this->r_pipe << endl;
		cout << "Output Schema: \n";
		set<Schema *, sch_comp> print_set;
		print_set.insert(this->tables[0]->second.sch);
		print_set.insert(this->tables[1]->second.sch);
		if(this->l_child->type & join_op) {
			for(int i=0; i<this->l_child->tables.size(); i++) {
				print_set.insert(this->l_child->tables[i]->
								second.sch);
			}
		}
		if(this->r_child->type & join_op) {
			for(int i=0; i<this->r_child->tables.size(); i++) {
				print_set.insert(this->r_child->tables[i]->
								second.sch);
			}
		}
		cout << "Set Size: " << print_set.size() << endl;
		for(auto i: print_set)
			i->Print();
	} else if(this->type & sel_file) {
		cout << "SELECT FILE\n";
		if(this->a_list==NULL && this->parent->l_child==this)
			this->tables.push_back(this->parent->tables[0]);
		else if(this->a_list==NULL && this->parent->r_child==this)
			this->tables.push_back(this->parent->tables[1]);
		cout << "CNF: \n";
		Schema *sch=this->tables[0]->second.sch;
		cnf.GrowFromParseTree(this->a_list, sch, literal);
		cnf.Print();
		cout << "Output pipe ID: " << this->p_pipe << endl;
		cout << "Input pipe ID: ";
		cout << this->l_pipe << endl;
		cout << "Output Schema: \n";
		this->tables[0]->second.sch->Print();
	} else if(this->type & sel_pipe) {
		cout << "SELECT PIPE\n";
		cout << "CNF: \n";
		Schema *sch=this->tables[0]->second.sch;
		cnf.GrowFromParseTree(this->a_list, sch, literal);
		cnf.Print();
		cout << "Output pipe ID: " << this->p_pipe << endl;
		cout << "Input pipe ID: ";
		cout << this->l_pipe << endl;
		cout << "Output Schema: \n";
		this->tables[0]->second.sch->Print();
	}
	cout << "**********\n";
}

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
			sel_op->l_pipe=-2;
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
		sel_op=new operation(sel_file);
		sel_flag=1;
		sel_op->l_pipe=-2;
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
				cerr << "Error in making tree!\n";
				_exit(-1);
			}
			tree_stk.push(op);
		}
	}
	this->tree=tree_stk.top();
	cout << "Printing the tree in order!\n";
	print_in_order(this->tree);
}

void print_in_order(struct operation *tree)
{
	if(tree->l_child!=NULL)
		//go left
		print_in_order(tree->l_child);

	//print
	tree->print();

	if(tree->r_child!=NULL)
		//go right
		print_in_order(tree->r_child);
}
