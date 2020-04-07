#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>

#include "statistics.h"

relInfo& relInfo :: operator=(relInfo& in)
{
	relInfo& curr_ref=*this;
	curr_ref.numTuples=in.numTuples;
	curr_ref.numRel=in.numRel;
	curr_ref.attrs=in.attrs;
	return curr_ref;
}

relInfo :: relInfo()
{

}

relInfo :: ~relInfo()
{

}

Statistics :: Statistics()
{

}

Statistics :: Statistics(Statistics &copyMe)
{
	for (auto& iter: copyMe.relMap) {
		string str1 = iter.first;
		relInfo relinfo;
		relinfo.numTuples = iter.second.numTuples;
		relinfo.numRel = iter.second.numRel;

		for (auto& iter_1: iter.second.attrs) {
			string str2 = iter_1.first;
			int n = iter_1.second;
			relinfo.attrs.insert(pair<string, int>(str2, n));
		}

		relMap.insert(pair<string, relInfo>(str1, relinfo));
		relinfo.attrs.clear();
	}
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
	auto iter = relMap.find(string(rel_name));
	if(!strcmp(iter->first.c_str(), rel_name))
		//relation DNE
		return;

	//NOTE
	//If num_distincts == -1, this means all the values are distinct, and
	//hence it turns out to be the total number of tuples
	//So this attribute is most probably a key and hence must be on the left
	//side of the join
	if (num_distincts == -1)
		num_distincts = iter->second.numTuples;

	iter->second.attrs.insert(pair<string, int>(string(att_name),
								num_distincts));
}

void Statistics :: CopyRel(char *oldName, char *newName)
{
	auto iter = relMap.find(string(oldName));
	if(!strcmp(iter->first.c_str(), oldName))
		//relation DNE
		return;

	relInfo newRel;

	newRel.numTuples = iter->second.numTuples;
	newRel.numRel = iter->second.numRel;

	for (auto& iter_1: iter->second.attrs){
		char *newAttrs = new char[200];
		sprintf(newAttrs, "%s.%s", newName, iter_1.first);
		newRel.attrs.insert(pair<string, int>(string(newAttrs),
							iter_1.second));
	}

	relMap.insert(pair<string, relInfo>(string(newName), newRel));
}

FILE *Statistics :: f_handle(char *fname, const char *perm)
{
	FILE *f=NULL;
	struct stat buf;
	int ret=stat(fname, &buf);
	int flag=1;
	if(!ret && !strcmp(perm, "w")) {
		cerr << "File " << fname << " exists"
			<< " and is being over written!\n";
	} else if(ret && !strcmp(perm, "r")) {
		cerr << "File " << fname << " doesnt exist, "
			<< "creating a new one!\n";
		if((f=this->f_handle(fname, "w"))==NULL)
			_exit(-1);
		flag=0;
	}

	if(flag) {
		if((f=fopen(fname, perm))==NULL) {
			cerr << "Error in opening file "
				<< fname << endl;
			return NULL;
		}
	}

	return f;
}

void Statistics :: Read(char *fname)
{
	FILE *f=NULL;
	if((f=this->f_handle(fname, "r"))==NULL)
		_exit(-1);

	char *line=NULL;
	size_t n=0;
	struct relInfo curr_rel;
	char curr_rel_name[64];
	int begin=1;
	while(!feof) {
		getline(&line, &n, f);
		char *l_point=strtok(line, ":");
		if(!strcmp(l_point, "R_BEGIN")) {
			if(!begin) {
				this->relMap.insert(pair<string, relInfo>
							(string(curr_rel_name),
							curr_rel));
				explicit_bzero(curr_rel_name, 64*sizeof(char));
			} else {
				begin=0;
			}
			sprintf(curr_rel_name, "%s", strtok(NULL, ":"));
			curr_rel.numTuples=strtol(strtok(NULL, ":"), NULL, 10);
			curr_rel.numRel=strtol(strtok(NULL, ":"), NULL, 10);
		} else if(!strcmp(l_point, "A_BEGIN")) {
			char *a_name=strtok(NULL, ":");
			int num=strtol(strtok(NULL, ":"), NULL, 10);
			curr_rel.attrs.insert(pair<string, int>(string(a_name),
								num));
		}
		free(line);
	}

	fclose(f);
}

void Statistics :: Write(char *fname)
{
	FILE *f=NULL;
	if((f=this->f_handle(fname, "w"))==NULL)
		_exit(-1);

	for(auto& i: this->relMap) {
		fprintf(f, "R_BEGIN:%s:%d:%d\n",
			i.first, i.second.numTuples, i.second.numRel);
		for(auto& j: i.second.attrs) {
			fprintf(f, "A_BEGIN:%s:%d", j.first, j.second);
		}
	}

	fclose(f);
}

/*
void  Statistics :: Apply(struct AndList *parseTree, char *relNames[],
			int numToJoin)
{

}
double Statistics :: Estimate(struct AndList *parseTree, char **relNames,
				int numToJoin)
{

}
*/
