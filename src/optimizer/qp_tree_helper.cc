#include<string.h>
#include<unistd.h>

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
		double cost=this->s->Estimate(alist);
		struct ComparisonOp *cop=olist->left;
		struct Operand *op=cop->left;
		vector<tableInfo *> vec;
		while(vec.size()<3 && op->code==NAME) {
			char name[64];
			sprintf(name, "%s", op->value);
			char *rel=strtok(name, ".");
			if(!strcmp(rel, op->value)) {
				cerr << "Bad attribute: " << op->value << endl;
				_exit(-1);
			}
			auto itr=this->relations.find(string(rel));
			vec.push_back(itr->second);
			op=cop->right;
		}
		if(vec.size()==2) {
			struct operation *op=new operation(join_f, cost, vec);
			op->join.alist=alist;
			this->join_queue.push(op);
		} else {
			vec[0]->add_sel(alist, cost);
		}
	}

	if(olist!=NULL && olist->rightOr!=NULL)
		this->process(alist, olist->rightOr);
}

void Qptree :: mk_ops(struct AndList *alist)
{
	//traverse to right most and make an op
	if(alist->rightAnd!=NULL)
		mk_ops(alist->rightAnd);

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
		this->mk_parent(op, ip, i);
	}

	if(i==op->tables.size())
		j_done.push_back(op);
}

struct operation *Qptree :: dispense_join(struct operation *op,
				vector<struct operation *> &j_done, int indx)
{
	struct operation *op_ret=NULL;
	for(int i=0; i<j_done.size(); i++) {
		if(op->tables[indx]->sch==j_done[i]->tables[0]->sch
			|| op->tables[indx]->sch==j_done[i]->tables[1]->sch) {
			op_ret=j_done[i];
			j_done.erase(j_done.begin() + i + 1);
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
}
