#include<iostream>
#include<string.h>

#include"statistics.h"

using namespace std;

relInfo :: relInfo() {}
relInfo :: ~relInfo() {}
relInfo &relInfo :: operator=(relInfo &in)
{
	this->numTuples=in.numTuples;
	this->relCount=in.relCount;

	return *(this);
}

attInfo :: attInfo() {}
attInfo :: ~attInfo() {}
attInfo &attInfo :: operator=(attInfo &in)
{
	this->relName=in.relName;
	this->n_distinct=in.n_distinct;

	return *(this);
}

Statistics :: Statistics() {}
Statistics :: Statistics(Statistics &in)
{
	for(auto &itr: in.relMap)
		this->relMap.insert(pair<string, relInfo>(itr.first,
								itr.second));
	for(auto &itr: in.attMap)
		this->attMap.insert(pair<string, attInfo>(itr.first,
								itr.second));
}
Statistics :: ~Statistics() {}
