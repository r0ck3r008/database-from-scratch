#include<iostream>
#include<string.h>

#include"statistics.h"

using namespace std;

relInfo :: relInfo(){}
relInfo &relInfo :: operator=(const relInfo &in)
{
	this->numTuples=in.numTuples;
	this->attMap=in.attMap;
	this->joins=in.joins;

	return *(this);
}
relInfo :: ~relInfo(){}

Statistics :: Statistics(){}
Statistics :: Statistics(Statistics &in)
{
	this->relMap=in.relMap;
}
Statistics :: ~Statistics(){}

void Statistics :: AddRel(char *_r_name, int numTuples)
{
	string r_name=string(_r_name);
	auto itr=this->relMap.find(r_name);
	if(itr!=this->relMap.end()) {
		cerr << "This relation already exists!\n";
		return;
	}

	relInfo r_info;
	r_info.numTuples=numTuples;
	r_info.joins.insert(r_name);
	this->relMap.insert(pair<string, relInfo>(r_name, r_info));
}

void Statistics :: AddAtt(char *_r_name, char *_a_name, int n_dis)
{
	string r_name=string(_r_name);
	string a_name=string(_a_name);

	auto itr1=this->relMap.find(r_name);
	if(itr1==this->relMap.end()) {
		cerr << "Such a relation does not exist!\n";
		return;
	}

	auto itr2=itr1->second.attMap.find(a_name);
	if(itr2!=itr1->second.attMap.end()) {
		cerr << "Such an attribute already exists!\n";
		return;
	}

	n_dis=(n_dis!=-1) ? (n_dis) : (itr1->second.numTuples);
	itr1->second.attMap.insert(pair<string, int>(a_name, n_dis));
}

void Statistics :: CopyRel(char *_o_name, char *_n_name)
{
	string o_name=string(_o_name);
	string n_name=string(_n_name);

	auto itr1=this->relMap.find(o_name);
	if(itr1==this->relMap.end()) {
		cerr << "This relation does not exist!\n";
		return;
	}

	relInfo r_info=itr1->second;
	r_info.joins.erase(o_name);
	r_info.joins.insert(n_name);
	this->relMap.insert(pair<string, relInfo>(n_name, r_info));
}

void Statistics :: Read(char *fname)
{
	FILE *f=NULL;
	if((f=this->f_handle(fname, "r"))==NULL)
		return;

	char *line=NULL;
	relInfo curr_rinfo;
	string curr_rname;
	size_t n=0;
	int begin=1;
	while(!feof(f)) {
		int stat=getline(&line, &n, f);
		if(stat==-1)
			break;
		char *start=strtok(line, ":");
		if(!strcmp(start, "R_BEGIN")) {
			if(!begin) {
				this->relMap.insert(pair<string, relInfo>
						(curr_rname, curr_rinfo));
			} else {
				begin=0;
			}
			curr_rname=string(strtok(NULL, ":"));
			curr_rinfo.numTuples=strtol(strtok(NULL, ":"),
								NULL, 10);
			char *join=strtok(NULL, ":");
			while(join!=NULL) {
				curr_rinfo.joins.insert(string(join));
				join=strtok(NULL, ":");
			}
		} else if(!strcmp(start, "A_BEGIN")) {
			string a_name=string(strtok(NULL, ":"));
			int n_dis=strtol(strtok(NULL, ":"), NULL, 10);
			curr_rinfo.attMap.insert(pair<string, int>
							(a_name, n_dis));
		}
	}
	this->relMap.insert(pair<string, relInfo>(curr_rname, curr_rinfo));

	free(line);
	fclose(f);
}

void Statistics :: Write(char *fname)
{
	FILE *f=NULL;
	if((f=this->f_handle(fname, "w"))==NULL)
		return;

	for(auto &i: this->relMap) {
		fprintf(f, "R_BEGIN:%s:%d",
			i.first.c_str(), i.second.numTuples);
		for(auto &j: i.second.joins)
			fprintf(f, ":%s", j.c_str());
		fprintf(f, "\n");

		for(auto &j: i.second.attMap)
			fprintf(f, "A_BEGIN:%s:%d\n",
				j.first.c_str(), j.second);
	}

	fclose(f);
}

void Statistics :: Apply(struct AndList *a_list, char **rel_names, int n)
{
	double ans=0.0;
	if(!this->traverse(a_list, NULL, &ans, rel_names, n, 1)) {
		cerr << "Error in applying!\n";
		return;
	}
}

double Statistics :: Estimate(struct AndList *a_list, char **rel_names, int n)
{
	double ans=0.0;
	if(!this->traverse(a_list, NULL, &ans, rel_names, n, 0)) {
		cerr << "Error in estimation\n";
		return -1;
	}

	return ans;
}
