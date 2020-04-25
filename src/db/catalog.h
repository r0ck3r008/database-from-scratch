#ifndef CATALOG_H
#define CATALOG_H

#include<unordered_map>

#include"glbl/defs.h"

struct attInfo
{
	Type type;
	int n_dis;
};

// Forced to used std::string over char * as stl plays nice with string not char
// *
struct relInfo
{
	std::unordered_map<std::string, attInfo> attMap;
	int n_att;
	int n_tup;
	fType type;
	std::string fname;

public:
	relInfo();
	relInfo(const relInfo &);
	~relInfo();
};

class Catalog
{
	std::unordered_map<std::string, relInfo> rels;

public:
	void addRel(char *, char *, fType, int);
	void addAtt(char *, char *, int, Type);
	relInfo *snap(char *);
	void write(char *);
	void read(char *);
	int Find(char *);
	Type FindType(char *);
};

void splice(char *, std::string *, std::string *);
FILE *f_handle(char *, const char *);

#endif
