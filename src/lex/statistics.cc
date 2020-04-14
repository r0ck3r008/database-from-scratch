#include<iostream>
#include<string.h>
#include<vector>
#include<unistd.h>

#include"statistics.h"

using namespace std;

relInfo :: relInfo() {}
relInfo :: ~relInfo() {}
relInfo &relInfo :: operator=(relInfo &in)
{
	this->numTuples=in.numTuples;
	this->joins=in.joins;

	return *(this);
}

Statistics :: Statistics() {}
Statistics :: Statistics(Statistics &in)
{
	for(auto &itr: in.relMap)
		this->relMap.insert(pair<string, relInfo>(itr.first,
								itr.second));
	for(auto &itr: in.attMap)
		this->attMap.insert(pair<string, int>(itr.first,
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
	r_info.joins.insert(r_name);
	this->relMap.insert(pair<string, relInfo>(r_name, r_info));
}

void Statistics :: AddAtt(char *_r_name, char *_a_name, int n_distinct)
{
	string r_name(_r_name);
	string a_name=r_name + "." + string(_a_name);
	auto itr=this->attMap.find(a_name);
	if(itr!=this->attMap.end() && !itr->first.compare(a_name)) {
		cerr << "Attribute under same relation already exists!\n";
		return;
	}
	auto itr1=this->relMap.find(r_name);
	if(itr1==this->relMap.end()) {
		cerr << "Such a relation doesnt exit!\n";
		return;
	}

	n_distinct=(n_distinct==-1) ? (itr1->second.numTuples) :
								(n_distinct);
	this->attMap.insert(pair<string, int>(a_name, n_distinct));
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
	//ugly still
	for(auto &i: this->attMap) {
		char a_name[64];
		sprintf(a_name, "%s", i.first.c_str());
		char *rel_name=strtok(a_name, ".");
		if(!strcmp(_o_name, rel_name)) {
			char *a_new_name=strtok(NULL, ".");
			string name=n_name + "." + string(a_new_name);
			this->attMap.insert(pair<string, int>
					(name, i.second));
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
			char *join_name=strtok(NULL, ":");
			while(join_name!=NULL) {
				r_info.joins.insert(string(join_name));
				join_name=strtok(NULL, ":");
			}
			this->relMap.insert(pair<string, relInfo>
						(string(r_name), r_info));

		} else if(!strcmp(type, "A_BEGIN")) {
			char *a_name=strtok(NULL, ":");
			int n_dis=strtol(strtok(NULL, ":"), NULL, 10);
			this->attMap.insert(pair<string, int>
					(string(a_name), n_dis));
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

	for(auto &itr: this->relMap) {
		fprintf(f, "R_BEGIN:%s:%d", itr.first.c_str(),
			itr.second.numTuples);
		for(auto &i: itr.second.joins)
			fprintf(f, ":%s", i);
		fprintf(f, "\n");
	}

	for(auto &itr: this->attMap)
		fprintf(f, "A_BEGIN:%s:%d\n", itr.first.c_str(),
							itr.second);

	fclose(f);
}

void Statistics :: Apply(AndList *parse_tree, char **rel_names, int n)
{
	double res=0.0;
	if(!this->traverse(parse_tree, NULL, &res, rel_names, n, 1)) {
		cerr << "Apply failed!\n";
		_exit(-1);
	}
}

double Statistics :: Estimate(AndList *parse_tree, char **rel_names, int n)
{
	double res=0.0;
	if(!this->traverse(parse_tree, NULL, &res, rel_names, n, 0)) {
		cerr << "Estimation failed!\n";
		_exit(-1);
	}

	return res;
}
