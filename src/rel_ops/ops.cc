#include<string.h>

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
	} else {
		char tblname[64], binname[64];
		sprintf(tblname, "%s", sch->fname.c_str());
		sprintf(binname, "bin/%s.bin", strtok(tblname, "."));
		DBFile *dbf=new DBFile;
		dbf->Open(binname);
		this->self=new SelectFile;
		self->Run(dbf, this->opipe, this->cnf, this->literal);
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
	} else {
		this->selp=new SelectPipe;
		this->selp->Run(this->ipipe, this->opipe, this->cnf,
								this->literal);
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
	} else {
		this->j=new Join;
		this->j->Run(this->ipipe1, this->ipipe2, this->opipe, this->cnf,
				this->literal, this->schl, this->schr);
	}
}
