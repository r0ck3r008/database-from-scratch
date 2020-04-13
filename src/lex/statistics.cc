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
	auto itr1=this->relMap.find(r_name);
	if(itr1==this->relMap.end()) {
		cerr << "Such a relation doesnt exit!\n";
		return;
	}

	int n_dist=(n_distinct==-1) ? (itr1->second.numTuples) : (n_distinct);
	itr1->second.attMap.insert(pair<string, int>(a_name, n_dist));
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
}

void Statistics :: Read(char *fname)
{
	//String to get rid of those pesky warnings
	FILE *f=this->f_handle(fname, string("r").c_str());
	char *line=NULL;
	string rname_curr;
	relInfo curr_rel;
	size_t n=0;
	int begin=1;
	while(!feof) {
		ssize_t stat=getline(&line, &n, f);
		if(stat==-1)
			break;

		char *type=strtok(line, ":");
		if(!strcmp(type, "R_BEGIN")) {
			if(!begin) {
				this->relMap.insert(pair<string, relInfo>
							(rname_curr, curr_rel));
				explicit_bzero(&curr_rel, sizeof(relInfo));
			} else {
				begin=0;
			}
			rname_curr=string(strtok(NULL, ":"));
			curr_rel.numTuples=strtol(strtok(NULL, ":"), NULL, 10);
			curr_rel.relCount=strtol(strtok(NULL, ":"), NULL, 10);
		} else if(!strcmp(type, "A_BEGIN")) {
			string a_name(strtok(NULL, ":"));
			int distinct=strtol(strtok(NULL, ":"), NULL, 10);
			curr_rel.attMap.insert(pair<string, int>
							(a_name, distinct));
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

	for(auto &i: this->relMap) {
		fprintf(f, "R_BEGIN:%s:%d:%d\n", i.first.c_str(),
			i.second.numTuples, i.second.relCount);
		for(auto &itr: i.second.attMap) {
			fprintf(f, "A_BEGIN:%s:%d\n", itr.first.c_str(),
							itr.second);
		}
	}

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
