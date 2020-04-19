#ifndef STATISTICS_H
#define STATISTICS_H

#include<unordered_map>
#include<vector>
#include<set>

#include"parser/parse_tree.h"

struct relInfo
{
	int numTuples;
	std :: unordered_map<std :: string, int> attMap;
	std :: set<std :: string> joins;

public:
	relInfo();
	~relInfo();
	relInfo &operator=(const relInfo &);
};

class Statistics
{
	std :: unordered_map<std :: string, relInfo> relMap;

private:
	FILE *f_handle(char *, const char *);
	int get_rels(std :: vector<std ::
		unordered_map<std :: string, relInfo> ::
		iterator> &, struct ComparisonOp *, char **,
		int);
	int traverse(struct AndList *, struct OrList *,
			double *, char **, int, int);
	void join_op(struct ComparisonOp *, double *,
		std :: vector<std :: unordered_map
		<std :: string, relInfo> :: iterator> &, int);
	void sel_op(struct ComparisonOp *, double *,
		std :: vector<std :: unordered_map
		<std :: string, relInfo> :: iterator> &);

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
	double Estimate(struct AndList *, char **, int);

};

#endif
