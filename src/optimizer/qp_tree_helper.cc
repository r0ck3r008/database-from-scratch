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
		t_info.qpt=this;
		t_info.sel_flag=0;
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

struct operation *Qptree :: dispense_join(struct operation *j_op, int indx,
					vector<operation *> &j_vec,
					stack<operation *> &j_stk)
{
	int flag=0;
	int i=0;
	struct operation *holder=NULL;
	for(i; i<j_vec.size(); i++) {
		if(!(j_op->tables[indx]->first.compare(
					j_vec[i]->tables[indx]->first))) {
			holder=j_vec[i];
			flag=1;
			break;
		}
	}

	if(flag) {
		j_stk.push(holder);
		j_vec.erase(j_vec.begin()+i);
	}
	return holder;
}

void Qptree :: process_join(struct operation *j_op, vector<operation *> &j_vec,
					stack<operation *> &j_stk)
{
	int flag=1;
	for(int i=0; i<j_op->tables.size(); i++) {
		struct operation *ip;
		if((ip=j_op->tables[i]->second.dispense_select())==NULL) {
			if((ip=this->dispense_join(j_op, i, j_vec, j_stk))
									==NULL) {
				flag=0;
				continue;
			}
		}

		int pipe=this->dispense_pipe();
		if(!i) {
			j_op->l_child=ip;
			j_op->l_pipe=pipe;
		} else {
			j_op->r_child=ip;
			j_op->r_pipe=pipe;
		}

		ip->parent=j_op;
		ip->p_pipe=pipe;
	}

	if(flag) {
		if(this->join_queue.empty())
			//last join op
			j_stk.push(j_op);
		else
			j_vec.push_back(j_op);
	}
}
