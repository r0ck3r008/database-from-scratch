#ifndef SCHEMA_H
#define SCHEMA_H

#include<iostream>
#include<unordered_map>

#include"glbl/defs.h"

struct Attribute
{
	char *name;
	Type myType;
	int n_dis;
};

struct Schema
{
	int numAtts;
	Attribute *myAtts;
	std::unordered_map<std::string,
		Attribute *> attMap;
	std::string fname;
	fType type;
	int n_tup;

	Schema();
	Schema(const Schema &);
	Schema(std::string, fType, int);
	Schema (char *, char *);
	Schema (char *, int , Attribute *);
	~Schema ();
	void addAtt(char *, Type, int);
	Attribute *GetAtts ();
	int GetNumAtts ();
	int Find (char *);
	Type FindType (char *);
	void Print();
};

#endif
