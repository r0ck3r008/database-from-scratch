#ifndef SCHEMA_H
#define SCHEMA_H

#include <stdio.h>
#include "fs/record.h"
#include "fs/file.h"
#include "lex/comparison.h"
#include "lex/comparison_engine.h"

struct Attribute {

	char *name;
	Type myType;
};

class OrderMaker;
class Schema {

	// gives the attributes in the schema
	int numAtts;
	Attribute *myAtts;

	// gives the physical location of the binary file storing the relation
	char *fileName;

	friend class Record;

public:

	// gets the set of attributes, but be careful with this, since it leads
	// to aliasing!!!
	Attribute *GetAtts ();

	// returns the number of attributes
	int GetNumAtts ();

	// this finds the position of the specified attribute in the schema
	// returns a -1 if the attribute is not present in the schema
	int Find (const char *);

	// this finds the type of the given attribute
	Type FindType (const char *);

	// this reads the specification for the schema in from a file
	Schema (const char *, const char *);

	// this constructs a sort order structure that can be used to
	// place a lexicographic ordering on the records using this type of schema
	int GetSortOrder (OrderMaker);

	~Schema ();

};

#endif
