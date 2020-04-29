#ifndef DDL_H
#define DDL_H

#include"glbl/defs.h"
#include"catalog.h"
#include"query.h"

class Ddl
{
	struct query *q;
	char *cat_file;

public:
	Ddl(struct query *, char *);
	~Ddl();
	void process();
	void create();
	void drop();
	void insert();
};

#endif
