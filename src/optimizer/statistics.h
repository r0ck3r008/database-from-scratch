#ifndef STATISTICS_H
#define STATISTICS_H

#include<map>
#include<unordered_map>
#include<vector>
#include<set>

#include"parser/parse_tree.h"

struct relInfo
{
	int numTuples;
	std :: set<std :: string> joins;

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
	int get_rels(std :: vector<std :: unordered_map
			<std :: string, relInfo> ::
			iterator> &, char **, int);
	void get_attrs(std :: vector<std :: unordered_map
			<std ::string, int> :: iterator> &,
			std :: vector<std :: unordered_map<
			std :: string, relInfo> :: iterator> &,
			std :: vector<std :: unordered_map<
			std :: string, relInfo> :: iterator> &,
			char *);
	int traverse(AndList *, OrList *, double *,
			char **, int, int);
	void join_op(double *,
		std :: vector<std :: unordered_map<std ::
		string, relInfo> :: iterator> &,
		std :: vector<std :: unordered_map<std ::
		string, int> :: iterator> &,
		int);
	void sel_op(ComparisonOp *, double *,
		std :: vector<std :: unordered_map<std ::
		string, relInfo> :: iterator> &,
		std :: vector<std :: unordered_map<std ::
		string, int> :: iterator> &);

public:
	Statistics();
	Statistics(Statistics &);
	Statistics(char *);
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
