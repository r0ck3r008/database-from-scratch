#include<iostream>

#include"operation.h"

using namespace std;

operation :: operation(type_flag type, double cost, vector<tableInfo *> &vec)
{
	this->lid=-1; this->rid=-1; this->pid=-1;
	this->lchild=NULL; this->rchild=NULL; this->parent=NULL;
	this->cost=cost;
	this->type=type;
	this->tables=vector<tableInfo *>(vec);
	this->oschl=new Schema;
	this->oschr=new Schema;
}

operation :: operation(type_flag type, Schema *sch)
{
	this->lid=-1; this->rid=-1; this->pid=-1;
	this->lchild=NULL; this->rchild=NULL; this->parent=NULL;
	this->cost=0;
	this->type=type;
	this->oschl=sch;
	this->oschr=new Schema;
}

operation :: ~operation(){}

void operation :: add_pipe(pipe_type p_type, Pipe *pipe)
{
	if(this->type & self_f) {
		if(p_type & parent_out)
			this->self.opipe=pipe;
		else
			cerr << "Unknown pipe type for select file op!\n";
	} else if(this->type & selp_f) {
		if(p_type & left_in)
			this->selp.ipipe=pipe;
		else if(p_type & parent_out)
			this->selp.opipe=pipe;
		else
			cerr << "Unknown pipe type for selectPipe!\n";
	} else if(this->type & join_f) {
		if(p_type & left_in)
			this->join.ipipe1=pipe;
		else if(p_type & right_in)
			this->join.ipipe2=pipe;
		else if(p_type & parent_out)
			this->join.opipe=pipe;
	} else if(this->type & grpby_f) {
		if(p_type & left_in)
			this->grpby.ipipe=pipe;
		else if(p_type & parent_out)
			this->grpby.opipe=pipe;
		else
			cerr << "Unknown pipe type for Group By!\n";
	} else if(this->type & sum_f) {
		if(p_type & left_in)
			this->sum.ipipe=pipe;
		else if(p_type & parent_out)
			this->sum.opipe=pipe;
		else
			cerr << "Unknown pipe type for Sum!\n";
	} else if(this->type & dist_f) {
		if(p_type & left_in)
			this->dist.ipipe=pipe;
		else if(p_type & parent_out)
			this->dist.opipe=pipe;
		else
			cerr << "Unknown pipe type for Distinct!\n";
	}
}

void operation :: append_sch(int indx, struct operation *child)
{
	if(child->oschl->numAtts)
		*(this->oschl)=*(this->oschl)+*(child->oschl);
	if(child->oschr->numAtts)
		*(this->oschr)=*(this->oschr)+*(child->oschr);

	if(!indx) {
		for(int i=0; i<child->tables.size(); i++) {
			if(child->tables[i]->sch!=this->tables[0]->sch)
				*(this->oschl)=*(this->oschl)+
						*(child->tables[i]->sch);
		}
	} else {
		for(int i=0; i<child->tables.size(); i++) {
			if(child->tables[i]->sch!=this->tables[1]->sch)
				*(this->oschr)=*(this->oschr)+
						*(child->tables[i]->sch);
		}
	}
}

void operation :: traverse(int indx)
{
	if(this->lchild!=NULL)
		this->lchild->traverse(indx);

	if(this->type & self_f)
		this->self.traverse(indx, this);
	else if(this->type & selp_f)
		this->selp.traverse(indx, this);
	else if(this->type & join_f)
		this->join.traverse(indx, this);
	else if(this->type & grpby_f)
		this->grpby.traverse(indx, this);
	else if(this->type & sum_f)
		this->sum.traverse(indx, this);
	else if(this->type & dist_f)
		this->dist.traverse(indx, this);

	if(!indx) {
		cout << "Input pipes: " << lid << " " << rid << endl;
		cout << "Output pipe: " << pid << endl;
	}

	if(this->rchild!=NULL)
		this->rchild->traverse(indx);
}
