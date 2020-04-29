#include<string.h>

#include"ops.h"
#include"optimizer/operation.h"

self_op :: self_op()
{
	this->alist=NULL; this->cnf=NULL; this->literal=NULL;
	this->opipe=NULL; this->self=NULL;
}
self_op :: ~self_op(){}

void self_op :: traverse(int indx, struct operation *parent)
{
	this->cnf=new CNF;
	this->literal=new Record;
	this->sch=parent->tables[0]->sch;
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

void selp_op :: traverse(int indx, struct operation *parent)
{
	this->cnf=new CNF;
	this->literal=new Record;
	this->sch=parent->tables[0]->sch;
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

void join_op :: traverse(int indx, struct operation *parent)
{
	this->cnf=new CNF;
	this->literal=new Record;
	this->schl=new Schema;
	this->schr=new Schema;
	*(this->schl)=*(parent->oschl)+*(parent->tables[0]->sch);
	*(this->schr)=*(parent->oschr)+*(parent->tables[1]->sch);
	this->cnf->GrowFromParseTree(this->alist, schl, schr, *(this->literal));
	if(!indx) {
		cout << "**********\n";
		cout << "JOIN:\n";
		this->cnf->Print();
		cout << "Left Schema:\n";
		this->schl->Print();
		cout << "Right Schema:\n";
		this->schr->Print();
		cout << "Output Schema:\n";
		Schema sch=*(this->schl)+*(this->schr);
		sch.Print();
		OrderMaker o_left, o_right;
		this->cnf->GetSortOrders(o_left, o_right);
		cout << "OrderMaker left:\n";
		o_left.Print();
		cout << "OrderMaker Right:\n";
		o_right.Print();
	} else {
		this->j=new Join;
		this->j->Run(this->ipipe1, this->ipipe2, this->opipe, this->cnf,
				this->literal, this->schl, this->schr);
	}
}

grpby_op :: grpby_op()
{
	this->grp=NULL; this->order=NULL;
	this->f=NULL; this->flist=NULL;
	this->ipipe=NULL; this->opipe=NULL;
	this->sch=NULL;
}

grpby_op :: ~grpby_op(){}

void grpby_op :: traverse(int indx, struct operation *parent)
{
	if(!indx) {
		cout << "*********\n";
		cout << "GROUP BY:\n";
		cout << "Function:\n";
		this->f->Print();
		cout << "Order:\n";
		this->order->Print();
		cout << "Input Schema:\n";
		parent->oschl->Print();
		cout << "Output Schema:\n";
		this->sch->Print();
	} else {
		this->grp=new GroupBy;
		this->grp->Run(this->ipipe, this->opipe, this->order, this->f);
	}
}

sum_op :: sum_op()
{
	this->s=NULL; this->f=NULL;
	this->flist=NULL; this->ipipe=NULL; this->opipe=NULL;
}
sum_op :: ~sum_op(){}

void sum_op :: traverse(int indx, struct operation *parent)
{
	if(!indx) {
		cout << "**********\n";
		cout << "SUM:\n";
		cout << "Function:\n";
		f->Print();
		cout << "Input Schema: \n";
		parent->oschl->Print();
		cout << "Output Schema:\n";
		this->sch->Print();
	} else {
		this->s=new Sum;
		s->Run(this->ipipe, this->opipe, this->f);
	}
}

dist_op :: dist_op()
{
	this->dist=NULL; this->sch=NULL; this->ipipe=NULL; this->opipe=NULL;
}
dist_op :: ~dist_op(){}

void dist_op :: traverse(int indx, struct operation *op)
{
	if(!indx) {
		cout << "**********\n";
		cout << "DUPLICATE REMOVAL:\n";
		cout << "Output Schema:\n";
		this->sch->Print();
	} else {
		this->dist=new DuplicateRemoval;
		dist->Run(this->ipipe, this->opipe, this->sch);
	}
}

proj_op :: proj_op()
{
	this->proj=NULL; this->ipipe=NULL; this->opipe=NULL;
	this->keep=NULL; this->atts_in=0; this->atts_out=0;
	this->sch=NULL;
}
proj_op :: ~proj_op(){}

void proj_op :: traverse(int indx, struct operation *parent)
{
	if(!indx) {
		cout << "**********\n";
		cout << "PROJECT:\n";
		cout << "Input Schema:\n";
		parent->oschl->Print();
		cout << "OutPut Schema:\n";
		this->sch->Print();
	} else {
		this->proj=new Project;
		proj->Run(this->ipipe, this->opipe, this->keep, this->atts_in,
				this->atts_out);
	}
}
