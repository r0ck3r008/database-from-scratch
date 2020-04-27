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
	this->rels=vector<tableInfo *>(vec);
	// add schema
	if(this->type & self_f) {
		this->self.sch=vec[0]->sch;
	} else if(this->type & selp_f) {
		this->selp.sch=vec[0]->sch;
	} else if(this->type & join_f) {
		this->join.schl=vec[0]->sch;
		this->join.schr=vec[1]->sch;
	}
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
			cerr << "Unknown pipe type for select pipe op!\n";
	} else if(this->type & join_f) {
		if(p_type & left_in)
			this->join.ipipe1=pipe;
		else if(p_type & right_in)
			this->join.ipipe2=pipe;
		else if(p_type & parent_out)
			this->join.opipe=pipe;
	}
}

void operation :: traverse(int indx)
{
	if(this->lchild!=NULL)
		this->lchild->traverse(indx);

	if(this->type & self_f)
		this->self.traverse(indx);
	else if(this->type & selp_f)
		this->selp.traverse(indx);
	else if(this->type & join_f)
		this->join.traverse(indx);

	if(!indx) {
		cout << "Input pipes: " << lid << " " << rid << endl;
		cout << "Output pipe: " << pid << endl;
	}

	if(this->rchild!=NULL)
		this->rchild->traverse(indx);
}
