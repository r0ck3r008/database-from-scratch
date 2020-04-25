#ifndef SCHEMA_H
#define SCHEMA_H

#include<unordered_map>

#include"glbl/defs.h"
//#include "comparator/comparison.h"

struct Attribute
{
	int n_dis;
	Type myType;
};

struct Schema
{
	std::string fname;
	int n_tup;
	fType type;
	std::unordered_map<std::string, Attribute> attMap;

	Schema();
	~Schema ();
	int Find (char *attName);
	Type FindType (char *attName);
//	int GetSortOrder (OrderMaker &order);
	void Print();
};

void splice(char *, std::string *, std::string *);

#endif
