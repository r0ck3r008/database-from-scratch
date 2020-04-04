#ifndef STATISTICS_H
#define STATISTICS_H
#include "parse_tree.h"
#include <map>
#include <string>

using namespace std;

typedef struct relInfo
{
	map<string, int> attrs;
	int numTuples;
	int numRel;
} relInfo;

class Statistics
{
	map<string,relInfo> relMap;
	double tempRes;

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
