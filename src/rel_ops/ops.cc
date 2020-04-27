#include"ops.h"

self_op :: self_op()
{
	this->alist=NULL; this->cnf=NULL; this->literal=NULL;
	this->opipe=NULL; this->self=NULL;
}
self_op :: ~self_op(){}

void self_op :: traverse(int indx)
{
	this->cnf=new CNF;
	this->literal=new Record;
	this->cnf->GrowFromParseTree(this->alist, sch, *(this->literal));
	if(!indx) {
		cout << "**********\n";
		cout << "SELECT FILE:\n";
		this->cnf->Print();
		this->sch->Print();
	}
}

selp_op :: selp_op()
{
	this->alist=NULL; this->cnf=NULL; this->literal=NULL;
	this->opipe=NULL; this->ipipe=NULL; this->selp=NULL;
}
selp_op :: ~selp_op(){}

void selp_op :: traverse(int indx)
{
	this->cnf=new CNF;
	this->literal=new Record;
	this->cnf->GrowFromParseTree(this->alist, sch, *(this->literal));
	if(!indx) {
		cout << "**********\n";
		cout << "SELECT PIPE:\n";
		this->cnf->Print();
		this->sch->Print();
	}
}

join_op :: join_op()
{
	this->alist=NULL; this->cnf=NULL; this->literal=NULL;
	this->opipe=NULL; this->ipipe1=NULL; this->j=NULL;
	this->ipipe2=NULL;
}
join_op :: ~join_op(){}

void join_op :: traverse(int indx)
{
	this->cnf=new CNF;
	this->literal=new Record;
	this->cnf->GrowFromParseTree(this->alist, schl, schr, *(this->literal));
	if(!indx) {
		cout << "**********\n";
		cout << "JOIN:\n";
		this->cnf->Print();
		this->schl->Print();
		this->schr->Print();
	}
}
