#include<iostream>
#include<string.h>
#include<unistd.h>
#include<errno.h>

#include"ddl.h"

using namespace std;

Ddl :: Ddl(struct query *q, char *cat_f)
{
	this->q=q;
	this->cat_file=cat_f;
}
Ddl :: ~Ddl(){}

void Ddl :: process()
{
	switch(q->query_type) {
	case 2:
		//create
		this->create();
		break;
	case 3:
		//drop
		this->drop();
		break;
	case 4:
		//insert
		this->insert();
		break;
	default:
		cerr << "Wrong query Type!\n";
	}
}

void Ddl :: create()
{
	Catalog c;
	struct AttrList *curr=q->atts_create;
	fType ftype;
	if(q->atts_sort!=NULL)
		ftype=Sorted;
	else
		ftype=Heap;
	char fname[64];
	sprintf(fname, "tmp/%s.bin", q->table);
	c.addRel(q->table, fname, ftype);
	int i=0;
	while(curr!=NULL) {
		int key=0;
		if(!i)
			key=1;
		c.addAtt(q->table, curr->name, (Type)curr->type, key);
		curr=curr->next;
		i++;
	}

	c.write(this->cat_file);
}

void Ddl :: drop()
{
	Catalog c;
	c.read(this->cat_file);

	Schema *sch=c.snap(q->table);
	if(sch==NULL)
		return;

	const char *fname=sch->fname.c_str();
	if(strlen(fname)==0) {
		cerr << "Unable to find db file, Did you insert?\n";
	} else {
		int stat=unlink(fname);
		if(stat==-1)
			cerr << "Error in unlinking the dbfile: "
				<< strerror(errno) << endl;
	}
	c.remRel(q->table);
	c.write(this->cat_file);
}

void Ddl :: insert()
{
	Catalog c;
	c.read(this->cat_file);
	Schema *sch=c.snap(this->q->table);
	if(sch==NULL)
		return;
	DBFile dbf;
	dbf.Create(sch->fname.c_str(), sch->type, NULL);
	dbf.Load(sch, sch->fname.c_str());
}
