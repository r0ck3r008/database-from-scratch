#ifndef STATISTICS_H
#define STATISTICS_H

#include<map>
#include<unordered_map>

#include"parse_tree.h"

struct relInfo
{
	std :: unordered_map<std :: string, int>
					attMap;
	int numTuples;
	int relCount;

public:
	relInfo();
	~relInfo();
	relInfo &operator=(relInfo &);
};

class Statistics
{
	std :: unordered_map<std :: string, relInfo>
					relMap;

private:
	FILE *f_handle(char *, const char *);
	double traverse(AndList *, OrList *, int);
	double join_op(ComparisonOp *, int);
	double sel_op(ComparisonOp *);

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
