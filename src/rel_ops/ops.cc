#include"ops.h"

self_op :: self_op()
{
	this->alist=NULL; this->cnf=NULL; this->literal=NULL;
	this->opipe=NULL; this->self=NULL;
}

self_op :: ~self_op(){}

selp_op :: selp_op()
{
	this->alist=NULL; this->cnf=NULL; this->literal=NULL;
	this->opipe=NULL; this->ipipe=NULL; this->selp=NULL;
}

selp_op :: ~selp_op(){}

join_op :: join_op()
{
	this->alist=NULL; this->cnf=NULL; this->literal=NULL;
	this->opipe=NULL; this->ipipe1=NULL; this->j=NULL;
	this->ipipe2=NULL;
}

join_op :: ~join_op(){}
