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

void Statistics :: AddRel(char *_r_name, int numTuples)
{
	string r_name(_r_name);
	auto itr=this->relMap.find(r_name);
	if(itr!=this->relMap.end()) {
		cerr << "Relation with same name exists!\n";
		return;
	}

	relInfo r_info;
	r_info.numTuples=numTuples;
	r_info.relCount=1;
	this->relMap.insert(pair<string, relInfo>(r_name, r_info));
}

void Statistics :: AddAtt(char *_r_name, char *_a_name, int n_distinct)
{
	string r_name(_r_name);
	string a_name(_a_name);
	auto itr=this->attMap.find(a_name);
	if(itr!=this->attMap.end() && !itr->second.relName.compare(r_name)) {
		cerr << "Attribute under same relation already exists!\n";
		return;
	}
	auto itr1=this->relMap.find(r_name);
	if(itr1==this->relMap.end()) {
		cerr << "Such a relation doesnt exit!\n";
		return;
	}

	attInfo a_info;
	a_info.n_distinct=(n_distinct==-1) ? (itr1->second.numTuples) :
								(n_distinct);
	a_info.relName=r_name;
	this->attMap.insert(pair<string, attInfo>(a_name, a_info));
}
