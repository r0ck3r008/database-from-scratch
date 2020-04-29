#include<algorithm>
#include<string.h>
#include<unistd.h>

#include"comparator/comparison.h"
#include"qp_tree.h"

using namespace std;

bool op_comp_join :: operator()(operation *l, operation *r)
{
	//min queue
	return (l->cost > r->cost);
}

void Qptree :: process(struct TableList *tbls)
{
	struct TableList *curr=tbls;
	while(curr!=NULL) {
		this->s->addRel(curr->tableName, curr->aliasAs);
		Schema *sch=this->c->snap(curr->tableName);
		string rname;
		if(curr->aliasAs==NULL)
			rname=string(curr->tableName);
		else
			rname=string(curr->aliasAs);
		tableInfo *tbl=new tableInfo;
		tbl->sch=sch;
		tbl->add_sel(NULL, 0);
		this->relations.insert(pair<string, tableInfo *>(rname, tbl));
		curr=curr->next;
	}
}

void Qptree :: process(struct AndList *alist, struct OrList *olist)
{
	if(olist==NULL && alist->left!=NULL) {
		this->process(alist, alist->left);
	} else {
		struct ComparisonOp *cop=olist->left;
		struct Operand *op=cop->left;
		vector<tableInfo *> vec;
		vector<int> vec_key;
		while(vec.size()<2 && op->code==NAME) {
			char name[64];
			sprintf(name, "%s", op->value);
			char *rel=strtok(name, ".");
			if(!strcmp(rel, op->value)) {
				cerr << "Bad attribute: " << op->value << endl;
				_exit(-1);
			}
			auto itr=this->relations.find(string(rel));
			auto itr2=itr->second->sch->attMap.find(
						string(strtok(NULL, ".")));
			vec_key.push_back(itr2->second->key);
			vec.push_back(itr->second);
			op=cop->right;
		}
		if(vec.size()==2) {
			if((!vec_key[0] && vec_key[1])) {
				//swap alist and vectors
				iter_swap(vec.begin(), vec.begin()+1);
				struct Operand *op=alist->left->left->left;
				alist->left->left->left=alist->left->left->right;
				alist->left->left->right=op;
			}
			double cost=this->s->Estimate(alist);
			struct operation *op=new operation(join_f, cost, vec);
			op->join.alist=alist;
			this->join_queue.push(op);
		} else {
			double cost=this->s->Estimate(alist);
			vec[0]->add_sel(alist, cost);
		}
	}

	if(olist!=NULL && olist->rightOr!=NULL)
		this->process(alist, olist->rightOr);
}

void Qptree :: mk_ops(struct AndList *alist)
{
	//traverse to right most and make an op
	if(alist==NULL)
		return;

	if(alist->rightAnd!=NULL)
		mk_ops(alist->rightAnd);

	if(alist->rightAnd!=NULL)
		alist->rightAnd=NULL;

	this->process(alist, NULL);
}

//process join ops
void Qptree :: process(struct operation *op, vector<struct operation *> &j_done)
{
	int i=0;
	for(i; i<op->tables.size(); i++) {
		struct operation *ip;
		struct tableInfo *tbl=op->tables[i];
		if((ip=tbl->dispense_sel(this))==NULL) {
			if((ip=this->dispense_join(op, j_done, i))==NULL) {
				cerr << "Error in dispensing select or join!\n";
				_exit(-1);
			}
		}
		mk_parent(this, op, ip, i);
	}

	if(i==op->tables.size()) {
		if(this->join_queue.size()==0)
			this->tree=op;
		else
			j_done.push_back(op);
	} else {
		cerr << "There is an error in processing join!\n";
	}
}

struct operation *Qptree :: dispense_join(struct operation *op,
				vector<struct operation *> &j_done, int indx)
{
	struct operation *op_ret=NULL;
	for(int i=0; i<j_done.size(); i++) {
		if(op->tables[indx]->sch==j_done[i]->tables[0]->sch
			|| op->tables[indx]->sch==j_done[i]->tables[1]->sch) {
			op_ret=j_done[i];
			j_done.erase(j_done.begin() + i);
			break;
		}
	}
	if(op_ret!=NULL)
		this->tree=op_ret;

	return op_ret;
}

void mk_parent(Qptree *qpt, struct operation *parent, struct operation *child,
									int indx)
{
	int pipe=0;
	Pipe *p=qpt->dispense_pipe(&pipe);
	child->parent=parent;
	child->pid=pipe;
	child->add_pipe(parent_out, p);
	if(!indx) {
		parent->lid=pipe;
		parent->lchild=child;
		parent->add_pipe(left_in, p);
	} else {
		parent->rid=pipe;
		parent->rchild=child;
		parent->add_pipe(right_in, p);
	}
	if(parent->tables.size())
		parent->append_sch(indx, child);

}

//grp_by
int Qptree :: process(struct NameList *grp_atts, struct FuncOperator *flist)
{
	if(grp_atts==NULL)
		return 0;
	Schema *sch=mk_agg_sch();
	struct NameList *curr=grp_atts;
	int stat=0;
	while(curr!=NULL) {
		stat=sch->Find(curr->name);
		if(stat==-1) {
			cerr << "Grouping attribute not found!\n";
			_exit(-1);
		} else {
			break;
		}
		curr=curr->next;
	}
	OrderMaker *order=new OrderMaker(sch, stat);
	Function *f=new Function;
	f->GrowFromParseTree(flist, *sch);
	struct operation *op=new operation(grpby_f, sch);
	mk_parent(this, op, this->tree, 0);
	op->grpby.order=order;
	op->grpby.f=f;
	op->grpby.flist=flist;
	op->grpby.sch=new Schema;
	op->grpby.sch->numAtts=1;
	op->grpby.sch->myAtts[0].update("Double", Double);
	this->tree=op;
	return 1;
}

//sum
void Qptree :: process(struct FuncOperator *flist)
{
	if(flist==NULL)
		return;
	Function *f=new Function;
	Schema *sch=mk_agg_sch();
	f->GrowFromParseTree(flist, *sch);
	struct operation *op=new operation(sum_f, sch);
	mk_parent(this, op, this->tree, 0);
	op->sum.f=f;
	op->sum.flist=flist;
	op->sum.sch=new Schema;
	op->sum.sch->numAtts=1;
	op->sum.sch->myAtts[0].update("Double", Double);
	this->tree=op;
}

//distinct
void Qptree :: process()
{
	Schema *sch=mk_agg_sch();
	struct operation *op=new operation(dist_f, sch);
	mk_parent(this, op, this->tree, 0);
	op->dist.sch=sch;
	this->tree=op;
}

//project
void Qptree :: process(struct NameList *sel_atts)
{
	if(sel_atts==NULL)
		return;
	Schema *sch=mk_agg_sch();
	struct operation *op=new operation(proj_f, sch);
	mk_parent(this, op, this->tree, 0);
	struct NameList *curr=sel_atts;
	int *keep=new int[16];
	int atts_out=0;
	op->proj.sch=new Schema;
	while(curr!=NULL) {
		int stat=sch->Find(curr->name);
		if(stat==-1) {
			cerr << "Error in finding projection attr!\n";
			_exit(-1);
		}
		Type type=sch->FindType(curr->name);
		op->proj.sch->myAtts[atts_out].update(curr->name, type);
		keep[atts_out]=stat;
		atts_out++;
		curr=curr->next;
	}
	op->proj.sch->numAtts=atts_out;
	op->proj.keep=keep;
	op->proj.atts_in=sch->numAtts;
	op->proj.atts_out=atts_out;
	this->tree=op;
}

Schema *Qptree :: mk_agg_sch()
{
	Schema *sch=new Schema;
	int size=this->tree->tables.size();
	if(this->tree->type & join_f) {
		if(this->tree->oschl->numAtts)
			*sch=*sch+*(this->tree->oschl);
		if(size)
			*sch=*sch+*(this->tree->tables[0]->sch);
		if(this->tree->oschr->numAtts)
			*sch=*sch+*(this->tree->oschr);
		if(size)
			*sch=*sch+*(this->tree->tables[1]->sch);
	} else {
		if(this->tree->oschl->numAtts) {
			if(this->tree->oschl->numAtts)
				*sch=*(this->tree->oschl);
			if(size)
				*sch=*sch+*(this->tree->tables[0]->sch);
		} else if(size)
			*sch=*(this->tree->tables[0]->sch);
	}

	return sch;
}
