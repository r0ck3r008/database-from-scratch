#ifndef STATISTICS_H
#define STATISTICS_H

#include<map>
#include<unordered_map>
#include<vector>
#include<set>
#include<unordered_set>

#include"parse_tree.h"

struct relInfo
{
	int numTuples;
	std :: vector<std :: string> attrs;
	std :: unordered_set<std :: string> joins;

public:
	relInfo();
	~relInfo();
	relInfo &operator=(relInfo &);
};

class Statistics
{
	std :: unordered_map<std :: string, relInfo>
					relMap;
	std :: unordered_map<std :: string, int>
					attMap;

private:
	FILE *f_handle(char *, const char *);
	int get_relations(std :: vector<std :: unordered_map
			<std :: string, relInfo> ::
			iterator> &, char **, int);
	int traverse(AndList *, OrList *, double *,
			char **, int, int);
	int join_op(ComparisonOp *, double *, char **,
					int, int);
	int sel_op(ComparisonOp *, double *,
					char **, int);

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
