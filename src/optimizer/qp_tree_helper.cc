#include<string.h>
#include<unistd.h>

#include"qp_tree.h"

using namespace std;

void mk_parent(Qptree *qpt, struct operation *parent, struct operation *child,
									int side)
{
	int pipe=qpt->dispense_pipe();
	child->p_pipe=pipe;
	child->parent=parent;
	if(!side) {
		//left child
		parent->l_child=child;
		parent->l_pipe=pipe;
	} else {
		parent->r_child=child;
		parent->r_pipe=pipe;
	}

	vector<unordered_map<string, tableInfo> :: iterator> tmp_vec;
	for(int i=0; i<child->curr_sch.size(); i++){
		int flag=1;
		for(int j=0; j<parent->curr_sch.size(); j++) {
			if(parent->curr_sch[j]==child->curr_sch[i])
				flag=0;
		}
		if(flag)
			tmp_vec.push_back(child->curr_sch[i]);
	}
	parent->curr_sch.insert(parent->curr_sch.end(), tmp_vec.begin(),
						tmp_vec.end());
	if(child->tables.size()==0 && parent->tables.size()!=0) {
		child->tables.push_back(parent->tables[side]);
		child->curr_sch.push_back(child->tables[0]);
	}
}

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
			op->curr_sch.push_back(p.second);
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

void Qptree :: process(struct operation *j_op, vector<operation *> &j_vec,
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
		mk_parent(this, j_op, ip, i);
	}

	if(flag) {
		if(this->join_queue.empty())
			//last join op
			j_stk.push(j_op);
		else
			j_vec.push_back(j_op);
	}
}

void Qptree :: process(struct NameList *grp_atts, struct NameList *sel_atts,
						struct FuncOperator *f_list)
{
	if(grp_atts==NULL)
		return;

	struct operation *op=new operation(grp_by);
	mk_parent(this, op, this->tree, 0);
	op->grp_sch=this->mk_sch(grp_atts, op);
	op->order=new OrderMaker(op->grp_sch);
	op->f_list=f_list;
	this->tree=op;
}

Schema *Qptree :: mk_sch(struct NameList *sel_atts, struct operation *op)
{
	struct NameList *curr=sel_atts;
	int num_atts=0;
	//max is 64
	struct Attribute atts[64];
	while(curr!=NULL) {
		int flag=0;
		for(int i=0; i<op->curr_sch.size(); i++) {
			Schema *sch=op->curr_sch[i]->second.sch;
			int indx=sch->Find(curr->name);
			if(indx!=-1) {
				atts[num_atts].myType=sch->FindType(curr->name);
				atts[num_atts].name=curr->name;
				flag=1;
				break;
			}
		}
		if(flag)
			num_atts++;
		curr=curr->next;
	}
	Schema *sch=new Schema("dummy2", num_atts, atts, 0);
	return sch;
}

void Qptree :: process(struct FuncOperator *f_list)
{
	if(f_list==NULL)
		return;

	struct operation *op=new operation(sum);
	mk_parent(this, op, this->tree, 0);
	op->f_list=f_list;
	this->tree=op;
}

void print_f_list(struct FuncOperator *f_list)
{
	if(f_list->leftOperator!=NULL)
		print_f_list(f_list->leftOperator);

	if(f_list->leftOperand!=NULL) {
		if(f_list->leftOperand->code==Int)
			cout << "Int: ";
		else if(f_list->leftOperand->code==Double)
			cout << "Double: ";
		else if(f_list->leftOperand->code==String)
			cout << "String: ";

		cout << f_list->leftOperand->value;

	}

	if(f_list->code=='+')
		cout << "+";
	else if(f_list->code=='-')
		cout << "-";
	else if(f_list->code=='*')
		cout << "*";
	else if(f_list->code=='/')
		cout << "/";

	if(f_list->right!=NULL)
		print_f_list(f_list->right);
}
