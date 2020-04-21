// TODO
// 1. Remove the priority queue and insert a vector instead, one for each select
// and join
// 2. Make a field in operation structure which basically includes all the
// required attributes for that operation
// 3. Start by examining each join, see what it requires and find the operation
// that can provide it, first priortiy is a select, then a join
// 4. Add those to make a tree
#include<string.h>
#include<unistd.h>

#include"qp_tree.h"

using namespace std;

void Qptree :: get_attr(char *att_name, pair<string, unordered_map<string,
			tableInfo> :: iterator> &p)
{
	char name[64];
	sprintf(name, "%s", att_name);
	string tbl=string(strtok(att_name, "."));
	string att=string(strtok(NULL, "."));

	auto itr=this->relations.find(tbl);
	if(itr==this->relations.end()) {
		cerr << "Attribute " << att_name << "not found!\n";
		_exit(-1);
	}

	p.first=att;
	p.second=itr;
}

void Qptree :: process(struct TableList *tables)
{
	struct TableList *curr=tables;
	while(curr!=NULL) {
		string r_name;
		Schema *sch;
		if(curr->aliasAs!=NULL) {
			this->s->CopyRel(curr->tableName, curr->aliasAs);
			r_name=string(curr->aliasAs);
			sch=new Schema(this->catalog_file, curr->aliasAs);
		} else {
			r_name=string(curr->tableName);
			sch=new Schema(this->catalog_file, curr->tableName);
		}
		struct tableInfo t_info;
		t_info.sch=sch;
		t_info.join_order=0;
		t_info.sel_order=0;
		this->relations.insert(pair<string, tableInfo>(r_name, t_info));

		curr=curr->next;
	}
}

void Qptree :: process(struct AndList *a_list, struct OrList *o_list,
						char **rels, int *curr_indx)
{
	if(o_list==NULL && a_list->left!=NULL) {
		this->process(a_list, a_list->left, rels, curr_indx);
	} else {
		struct ComparisonOp *cop=o_list->left;
		struct Operand *operand=cop->left;
		int n=0;
		while(n<2 && operand->code==NAME) {
			pair<string, unordered_map<string, tableInfo> ::
							iterator> p;
			this->get_attr(operand->value, p);
			rels[*curr_indx]=new char[64];
			sprintf(rels[*curr_indx], "%s", p.first.c_str());
			*curr_indx++;
			n++;
			operand=cop->right;
		}
		if(n==2)
			a_list->is_join=1;
		else
			a_list->is_join=0;
	}

	if(o_list!=NULL && o_list->rightOr!=NULL)
		this->process(a_list, o_list->rightOr, rels, curr_indx);
	else if(o_list==NULL && a_list->rightAnd!=NULL)
		this->process(a_list->rightAnd, NULL, rels, curr_indx);
}

void Qptree :: process(struct AndList *a_list)
{
	if(a_list->rightAnd!=NULL)
		this->process(a_list->rightAnd);

	struct operation op;
	op.a_list=a_list;
	if(a_list->rightAnd!=NULL)
		//assuming all the right Ands have already been dealt with
		a_list->rightAnd=NULL;

	//max 16 relations
	char **rels=new char *[16];
	int indx=0;
	this->process(a_list, NULL, rels, &indx);
	op.cost=this->s->Estimate(a_list, rels, indx);

	//assuming atomic operations per AndList
	for(int i=0; i<indx; i++) {
		string r_name=string(rels[i]);
		auto itr=this->relations.find(r_name);
		if(indx==2) {
			itr->second.join_order++;
			this->joins.push_back(op);
		} else {
			itr->second.sel_order++;
			this->selects.push_back(op);
		}
	}

	for(int i=0; i<indx; i++)
		delete[] rels[i];
	delete[] rels;
}
