#include<iostream>
#include<string.h>
#include<vector>

#include"statistics.h"

using namespace std;

relInfo :: relInfo() {}
relInfo :: ~relInfo() {}
relInfo &relInfo :: operator=(relInfo &in)
{
	this->attMap=in.attMap;
	this->numTuples=in.numTuples;
	this->relCount=in.relCount;

	return *(this);
}

Statistics :: Statistics() {}
Statistics :: Statistics(Statistics &in)
{
	this->relMap=in.relMap;
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

void Statistics :: CopyRel(char *_o_name, char *_n_name)
{
	string o_name(_o_name);
	string n_name(_n_name);
	auto itr=this->relMap.find(o_name);
	if(itr==this->relMap.end()) {
		cerr << "Relation with " << o_name << " doesnt exist!\n";
		return;
	}

	this->relMap.insert(pair<string, relInfo>(n_name, itr->second));
	vector<attInfo> pending;
	for(auto &itr: this->attMap) {
		if(itr.second.relName.compare(o_name)==0) {
			attInfo a_info=itr.second;
			a_info.relName=n_name;
			this->attMap.insert(pair<string, attInfo>(n_name,
								a_info));
		}
	}
}

void Statistics :: Read(char *fname)
{
	//String to get rid of those pesky warnings
	FILE *f=this->f_handle(fname, string("r").c_str());
	char *line=NULL;
	size_t n=0;
	while(!feof) {
		ssize_t stat=getline(&line, &n, f);
		if(stat==-1)
			break;

		char *type=strtok(line, ":");
		if(!strcmp(type, "R_BEGIN")) {
			relInfo r_info;
			char *r_name=strtok(NULL, ":");
			r_info.numTuples=strtol(strtok(NULL, ":"), NULL, 10);
			r_info.relCount=strtol(strtok(NULL, ":"), NULL, 10);
			this->relMap.insert(pair<string, relInfo>
						(string(r_name), r_info));

		} else if(!strcmp(type, "A_BEGIN")) {
			char *a_name=strtok(NULL, ":");
			char *r_name=strtok(NULL, ":");
			int num_distinct=strtol(strtok(NULL, ":"), NULL, 10);
			this->AddAtt(r_name, a_name, num_distinct);
		}

		free(line);
		line=NULL;
	}

	free(line);
	fclose(f);
}

void Statistics :: Write(char *fname)
{
	FILE *f=this->f_handle(fname, string("w").c_str());

	for(auto &itr: this->relMap)
		fprintf(f, "R_BEGIN:%s:%d:%d\n", itr.first.c_str(),
			itr.second.numTuples, itr.second.relCount);

	for(auto &itr: this->attMap)
		fprintf(f, "A_BEGIN:%s:%s:%d\n", itr.first.c_str(),
			itr.second.relName.c_str(), itr.second.n_distinct);

	fclose(f);
}

void Statistics :: Apply(AndList *parse_tree, char **rel_names, int n)
{
	this->traverse(parse_tree, NULL, 1);
}

double Statistics :: Estimate(AndList *parse_tree, char **rel_names, int n)
{
	return (this->traverse(parse_tree, NULL, 0));
}
