#include "statistics.h"

Statistics :: Statistics()
{

}

Statistics :: ~Statistics()
{

}

Statistics :: Statistics(Statistics &copyMe)
{
	for (map<string,relInfo>::iterator iter = copyMe.relMap.begin();
		iter!=copyMe.relMap.end(); ++iter) {
		string str1 = iter->first;
		relInfo relinfo;
		relinfo.numTuples = iter->second.numTuples;
		relinfo.numRel = iter->second.numRel;

		for (map<string, int>::iterator iter_1=
			iter->second.attrs.begin();
			iter_1!=iter->second.attrs.end(); ++iter_1) {
			string str2 = iter_1->first;
			int n = iter_1->second;
			relinfo.attrs.insert(pair<string, int>(str2, n));
		}

		relMap.insert(pair<string, relInfo>(str1, relinfo));
		relinfo.attrs.clear();
	}
}

void Statistics :: AddRel(char *relName, int numTuples)
{
	relInfo newRel;
	newRel.numTuples = numTuples;
	newRel.numRel = 1;
	string str(relName);
	relMap.insert(pair<string, relInfo>(str, newRel));
}

void Statistics :: AddAtt(char *relName, char *attName, int numDistincts)
{
	string rel(relName);
	string att(attName);
	map<string,relInfo>::iterator iter = relMap.find(rel);

	if (numDistincts == -1)
		numDistincts = iter->second.numTuples;//Why?

	iter->second.attrs.insert(pair<string, int>(att, numDistincts));
}

void Statistics :: CopyRel(char *oldName, char *newName)
{
	string oldRelName(oldName);
	string newRelName(newName);
	map<string,relInfo>::iterator iter = relMap.find(oldRelName);
	relInfo newRel;

	newRel.numTuples = iter->second.numTuples;
	newRel.numRel = iter->second.numRel;

	for (map<string,int>::iterator iter_1 = iter->second.attrs.begin();
		iter_1!=iter->second.attrs.end(); ++iter_1){
		char * newAttrs = new char[200];
		sprintf(newAttrs, "%s.%s", newName, iter_1->first.c_str());//Why?
		string temp(newAttrs);
		newRel.attrs.insert(pair<string,
					int>(newAttrs, iter_1->second));
	}

	relMap.insert(pair<string, relInfo>(newRelName, newRel));
}

void Statistics :: Read(char *fromWhere)
{

}

void Statistics :: Write(char *fromWhere)
{

}

void  Statistics :: Apply(struct AndList *parseTree, char *relNames[],
			int numToJoin)
{

}
double Statistics :: Estimate(struct AndList *parseTree, char **relNames,
				int numToJoin)
{

}
