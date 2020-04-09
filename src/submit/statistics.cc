#include<iostream>
#include<string.h>
#include<unistd.h>

#include "statistics.h"

relInfo :: relInfo(){}
relInfo :: ~relInfo(){}
relInfo& relInfo :: operator=(relInfo& in)
{
	relInfo& curr_ref=*this;
	curr_ref.numTuples=in.numTuples;
	curr_ref.numRel=in.numRel;
	return curr_ref;
}

attInfo :: attInfo(){}
attInfo :: ~attInfo(){}
attInfo& attInfo :: operator=(attInfo& in)
{
	attInfo& curr_ref=*this;
	curr_ref.rel_name=in.rel_name;
	curr_ref.num_distinct=in.num_distinct;

	return curr_ref;
}

Statistics :: Statistics(){}

Statistics :: Statistics(Statistics &copyMe)
{
	for(auto& itr: copyMe.relMap)
		relMap.insert(pair<string, relInfo>(itr.first, itr.second));

	for(auto& itr: copyMe.attrs)
		this->attrs.insert(pair<string, attInfo>(itr.first,
								itr.second));
}

Statistics :: ~Statistics()
{
}

void Statistics :: AddRel(char *rel_name, int num_tuples)
{
	struct relInfo newRel;
	newRel.numTuples = num_tuples;
	newRel.numRel = 1;
	this->relMap.insert(pair<string, relInfo>(string(rel_name), newRel));
}

void Statistics :: AddAtt(char *rel_name, char *att_name, int num_distincts)
{
	string r_name(rel_name);
	auto iter = relMap.find(r_name);
	if(iter==relMap.end())
		//relation DNE
		return;

	//NOTE
	//If num_distincts == -1, this means all the values are distinct, and
	//hence it turns out to be the total number of tuples
	//So this attribute is most probably a key and hence must be on the left
	//side of the join
	if (num_distincts == -1)
		num_distincts = iter->second.numTuples;

	attInfo attinfo;
	attinfo.rel_name=r_name;
	attinfo.num_distinct=num_distincts;
	string a_name=r_name + "." + string(att_name);
	this->attrs.insert(pair<string, attInfo>(a_name, attinfo));
}

void Statistics :: AddAtt(char *rel_name, char *att_name, int num_distincts,
				int flag)
{
	string r_name(rel_name);
	auto iter = relMap.find(r_name);
	if (num_distincts == -1)
		num_distincts = iter->second.numTuples;

	attInfo attinfo;
	attinfo.rel_name=r_name;
	attinfo.num_distinct=num_distincts;
	string a_name=string(att_name);
	this->attrs.insert(pair<string, attInfo>(a_name, attinfo));
}

void Statistics :: CopyRel(char *oldName, char *newName)
{
	auto itr = relMap.find(string(oldName));
	if(itr==relMap.end())
		//relation DNE
		return;

	relMap.insert(pair<string, relInfo>(string(newName), itr->second));
	this->CopyAttrs(oldName, newName);
}

void Statistics :: CopyAttrs(char *oldRel, char *newRel)
{
	for (auto& itr: this->attrs){
		if(itr.second.rel_name.compare(string(oldRel))==0) {
			//ugly
			char name[32];
			sprintf(name, "%s", itr.first.c_str());
			char *first=strtok(name, ".");
			char *second=strtok(NULL, ".");
			this->AddAtt(newRel, second,
					itr.second.num_distinct, 1);
			this->attrs.erase(itr.first);
		}
	}
}

void Statistics :: Read(char *fname)
{
	FILE *f=NULL;
	if((f=this->f_handle(fname, "r"))==NULL)
		_exit(-1);

	char *line=NULL;
	size_t n=0;
	while(1) {
		int stat=getline(&line, &n, f);
		if(stat==-1)
			break;
		char *l_point=strtok(line, ":");
		if(!strcmp(l_point, "R_BEGIN")) {
			relInfo relinfo;
			string rel_name=string(strtok(NULL, ":"));
			relinfo.numTuples=strtol(strtok(NULL, ":"), NULL, 10);
			relinfo.numRel=strtol(strtok(NULL, ":"), NULL, 10);
			this->relMap.insert(pair<string, relInfo> (rel_name,
								relinfo));
		} else if(!strcmp(l_point, "A_BEGIN")) {
			attInfo attinfo;
			string a_name=string(strtok(NULL, ":"));
			attinfo.rel_name=string(strtok(NULL, ":"));
			attinfo.num_distinct=strtol(strtok(NULL, ":"), NULL,
									10);
			attrs.insert(pair<string, attInfo>(string(a_name),
								attinfo));
		}
		free(line);
		line=NULL;
	}

	free(line);

	fclose(f);
}

void Statistics :: Write(char *fname)
{
	FILE *f=NULL;
	if((f=this->f_handle(fname, "w"))==NULL)
		_exit(-1);

	for(auto& i: this->relMap)
		fprintf(f, "R_BEGIN:%s:%d:%d\n",
			i.first.c_str(), i.second.numTuples, i.second.numRel);

	for(auto& i: this->attrs)
		fprintf(f, "A_BEGIN:%s:%s:%d\n",
			i.first.c_str(), i.second.rel_name.c_str(),
			i.second.num_distinct);

	fclose(f);
}


void  Statistics :: Apply(struct AndList *parseTree, char **relNames,
			int numToJoin)
{
	double res=0.0;
	this->stat_helper(parseTree, NULL, 1, &res);
}

double Statistics :: Estimate(struct AndList *parseTree, char **relNames,
				int numToJoin)
{
	double res=0.0;
	this->stat_helper(parseTree, NULL, 0, &res);

	return res;
}