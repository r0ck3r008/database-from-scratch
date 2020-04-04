#ifndef STATISTICS_
#define STATISTICS_
#include "parse_tree.h"
#include <map>
#include <string>

using namespace std;

typedef struct relInfo{
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
	Statistics(Statistics &copyMe);	 // Performs deep copy
	~Statistics();


	void AddRel(char *relName, int numTuples);
	void AddAtt(char *relName, char *attName, int numDistincts);
	void CopyRel(char *oldName, char *newName);
	
	void Read(char *fromWhere);
	void Write(char *fromWhere);

	void  Apply(struct AndList *parseTree, char *relNames[], int numToJoin);
	double Estimate(struct AndList *parseTree, char **relNames, int numToJoin);

};

#endif
