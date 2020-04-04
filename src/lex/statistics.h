#ifndef STATISTICS_H
#define STATISTICS_H
#include "parse_tree.h"
#include <map>
#include <unordered_map>
#include <string>

struct relInfo
{
	std :: unordered_map<char *, int> attrs;
	int numTuples;
	int numRel;
};

class Statistics
{
	std :: unordered_map<char *, relInfo> relMap;

private:
	FILE *f_handle(char *, const char *);

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
