#ifndef STATISTICS_H
#define STATISTICS_H

#include <map>
#include <unordered_map>
#include <string>

#include "parse_tree.h"

//safe as we do not have any other namespace
using namespace std;

struct relInfo
{
	int numTuples;
	int numRel;
//NOTE
//this is required to aid in inserting pairs to maps
//as they are replicated
public:
	relInfo();
	~relInfo();
	relInfo& operator=(relInfo&);
};

struct attInfo
{
	string rel_name;
	int num_distinct;

public:
	attInfo();
	~attInfo();
	attInfo& operator=(attInfo&);
};

class Statistics
{
	unordered_map<string, relInfo> relMap;
	unordered_map<string, attInfo> attrs;

private:
	FILE *f_handle(char *, const char *);
	double stat_helper(struct AndList *,
			struct OrList *, char **,
			int, int);

public:
	Statistics();
	// Performs deep copy
	Statistics(Statistics &copyMe);
	~Statistics();

	void AddRel(char *, int);
	void AddAtt(char *, char *,int);
	void CopyRel(char *, char *);

	void Read(char *);
	void Write(char *);

	void  Apply(struct AndList *, char **, int);
	double Estimate(struct AndList *, char **, int);

};

#endif
