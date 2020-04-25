#ifndef SCHEMA_H
#define SCHEMA_H

#include"glbl/defs.h"

struct Attribute {

	char *name;
	Type myType;
};

struct Schema
{
	int numAtts;
	Attribute *myAtts;
	char *fileName;

	Schema (char *, char *);
	Schema (char *, int , Attribute *);
	~Schema ();
	Attribute *GetAtts ();
	int GetNumAtts ();
	int Find (char *);
	Type FindType (char *);
	void Print();
};

#endif
