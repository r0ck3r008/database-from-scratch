#include<iostream>

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
		//this->drop();
		break;
	case 4:
		//insert
		//this->insert();
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
	c.addRel(q->table, ftype);
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
