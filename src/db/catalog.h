#ifndef CATALOG_H
#define CATALOG_H

#include<unordered_map>

#include"glbl/defs.h"
#include"schema.h"

class Catalog
{
	std::unordered_map<std::string, Schema> rels;

public:
	void addRel(char *, char *, fType, int);
	void addAtt(char *, char *, int, Type);
	Schema *snap(char *);
	void write(char *);
	void read(char *);
	int Find(char *);
	Type FindType(char *);
};

void splice(char *, std::string *, std::string *);
FILE *f_handle(char *, const char *);

#endif
