#include<string.h>
#include<unistd.h>

#include"qp_tree.h"

using namespace std;

void Qptree :: get_attr(char *att_name, pair<string, unordered_map<string,
			tableInfo> :: iterator> &p)
{
	char name[64];
	sprintf(name, "%s", att_name);
	string tbl=string(strtok(name, "."));
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
		Schema *sch=new Schema(this->catalog_file, curr->tableName);
		if(curr->aliasAs!=NULL) {
			this->s->CopyRel(curr->tableName, curr->aliasAs);
			r_name=string(curr->aliasAs);
		} else {
			r_name=string(curr->tableName);
		}
		struct tableInfo t_info;
		t_info.sch=sch;
		t_info.join_order=0;
		t_info.sel_order=0;
		this->relations.insert(pair<string, tableInfo>(r_name, t_info));

		curr=curr->next;
	}
}

void Qptree :: process(struct operation *op, struct AndList *a_list,
			struct OrList *o_list, char **rels, int *curr_indx)
{
	if(o_list==NULL && a_list->left!=NULL) {
		this->process(op, a_list, a_list->left, rels, curr_indx);
	} else {
		struct ComparisonOp *cop=o_list->left;
		struct Operand *operand=cop->left;
		int n=0;
		while(n<2 && operand->code==NAME) {
			pair<string, unordered_map<string, tableInfo> ::
							iterator> p;
			this->get_attr(operand->value, p);
			rels[*curr_indx]=new char[64];
			op->tables.push_back(p.second);
			sprintf(rels[*curr_indx], "%s",
					p.second->first.c_str());
			(*curr_indx)++;
			n++;
			operand=cop->right;
		}
		if(n==2) {
			op->type=join_op;
			a_list->is_join=1;
		} else {
			op->type=sel_any;
			a_list->is_join=0;
		}
	}

	if(o_list!=NULL && o_list->rightOr!=NULL)
		this->process(op, a_list, o_list->rightOr, rels, curr_indx);
	else if(o_list==NULL && a_list->rightAnd!=NULL)
		this->process(op, a_list->rightAnd, NULL, rels, curr_indx);
}

void Qptree :: process(struct AndList *a_list)
{
	if(a_list->rightAnd!=NULL)
		this->process(a_list->rightAnd);

	struct operation *op=new operation(a_list, this);
}

int Qptree :: dispense_pipe()
{
	return pipe_count++;
}

int Qptree :: dispense_join(struct operation **op_p)
{
	return 1;
}
