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
}

operation :: ~operation(){}
