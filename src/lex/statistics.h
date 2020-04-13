#ifndef STATISTICS_H
#define STATISTICS_H

#include<map>
#include<unordered_map>
#include<vector>

#include"parse_tree.h"

struct relInfo
{
	int numTuples;
	int relCount;
	std :: vector<std :: string> attrs;
	std :: vector<std :: string> joins;

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
	double traverse(AndList *, OrList *,
			char **, int, int);
	double join_op(ComparisonOp *, char **,
					int, int);
	double sel_op(ComparisonOp *, char **, int);

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
