#ifndef STATISTICS_H
#define STATISTICS_H

#include<map>
#include<unordered_map>

#include"parse_tree.h"

struct relInfo
{
	int numTuples;
	int relCount;

public:
	relInfo();
	~relInfo();
	relInfo &operator=(relInfo &);
};

struct attInfo
{
	std :: string relName;
	int n_distinct;

public:
	attInfo();
	~attInfo();
	attInfo &operator=(attInfo &);
};

class Statistics
{
	std :: unordered_map<std :: string, relInfo>
					relMap;
	std :: unordered_map<std :: string, attInfo>
					attMap;

private:
	FILE *f_handle(char *, const char *);
	double traverse(AndList *, OrList *, int);
	double operate(ComparisonOp *, int);

public:
	Statistics();
	Statistics(Statistics &);
	~Statistics();

	void AddRel(char *, int);
	void AddAtt(char *, char *, int);
	void CopyRel(char *, char *);
	void Read(char *);
	void Write(char *);

	void Apply(struct AndList *, char **, int);
	double Estimate(struct AndList *, char **,
					int);
};

#endif
