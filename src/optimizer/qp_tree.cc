#include"qp_tree.h"

bool operation_comp :: operator()(const operation &l, const operation &r)
{
	return (l.cost > r.cost);
}

Qptree :: Qptree(char *stat_fname, char *catalog_file)
{
	this->s=new Statistics;
	this->s->Read(stat_fname);
	this->catalog_file=catalog_file;
}

Qptree :: ~Qptree()
{
	delete this->s;
	for(auto &i: this->relations)
		delete i.second.sch;
	for(auto &i: this->op_list)
		delete i;
}

void Qptree :: process(struct query *q)
{
	this->process(q->tables);
	this->process(q->boolean);
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
			sprintf(rels[*curr_indx], "%s", p.first.c_str());
			*curr_indx++;
			n++;
			operand=cop->right;
		}
		if(n==2)
			op->n_join++;
	}

	if(o_list!=NULL && o_list->rightOr!=NULL)
		this->process(op, a_list, o_list->rightOr, rels, curr_indx);

	if(a_list->rightAnd!=NULL)
		this->process(op, a_list->rightAnd, NULL, rels, curr_indx);
}

void Qptree :: process(struct operation *op1, struct operation *op2)
{
	struct AndList *a_list=NULL;
	if(op2!=NULL) {
		if(op1->cost >= op2->cost) {
			op1->a_list->rightAnd=op2->a_list;
			a_list=op1->a_list;
		} else {
			op2->a_list->rightAnd=op1->a_list;
			a_list=op2->a_list;
		}
	} else {
		a_list=op1->a_list;
	}

	//max allowed 16 relations per query
	struct operation *op=new operation;
	op->a_list=a_list;
	op->n_join=0;
	char **rels=new char *[16];
	int indx=0;
	this->process(op, a_list, NULL, rels, &indx);
	op->cost=this->s->Estimate(a_list, rels, indx);
	delete op1;
	delete op2;

	//push to priority queue
}

void Qptree :: process(struct AndList *a_list)
{
	if(a_list->rightAnd!=NULL)
		this->process(a_list->rightAnd);

	struct operation *op=new operation;
	op->a_list=a_list;
	this->process(op, NULL);
}
