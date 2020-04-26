#include"operation.h"

operation :: operation(type_flag type)
{
	this->lid=-1; this->rid=-1; this->pid=-1;
	this->lchild=NULL; this->rchild=NULL; this->parent=NULL;
	this->type=type;
	this->cost=0;
}

operation :: ~operation(){}
