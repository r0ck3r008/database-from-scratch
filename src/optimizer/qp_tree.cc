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
	this->op_queue.push(op);

	//assuming atomic operations per AndList
	for(int i=0; i<indx; i++) {
		string r_name=string(rels[i]);
		auto itr=this->relations.find(r_name);
		if(indx==2)
			itr->second.join_order++;
		else
			itr->second.sel_order++;
	}
}
