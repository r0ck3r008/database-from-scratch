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

void operation :: append_sch(int indx, struct operation *child)
{
	for(int i=0; i<child->oschl.size(); i++)
		this->oschl.push_back(child->oschl[i]);
	for(int i=0; i<child->oschr.size(); i++)
		this->oschr.push_back(child->oschr[i]);

	if(!indx) {
		for(int i=0; i<child->tables.size(); i++) {
			if(child->tables[i]->sch!=this->tables[0]->sch)
				this->oschl.push_back(child->tables[i]);
		}
	} else {
		for(int i=0; i<child->tables.size(); i++) {
			if(child->tables[i]->sch!=this->tables[1]->sch)
				this->oschr.push_back(child->tables[i]);
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

	if(!indx) {
		cout << "Input pipes: " << lid << " " << rid << endl;
		cout << "Output pipe: " << pid << endl;
	}

	if(this->rchild!=NULL)
		this->rchild->traverse(indx);
}
