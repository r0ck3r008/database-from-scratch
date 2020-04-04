#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include "statistics.h"

Statistics :: Statistics(Statistics &copyMe)
{
	for (auto& iter: copyMe.relMap) {
		char *str1 = iter.first;
		relInfo relinfo;
		relinfo.numTuples = iter.second.numTuples;
		relinfo.numRel = iter.second.numRel;

		for (auto& iter_1: iter.second.attrs) {
			char *str2 = iter_1.first;
			int n = iter_1.second;
			relinfo.attrs.insert(std :: pair<char *, int>(str2, n));
		}

		relMap.insert(std :: pair<char *, relInfo>(str1, relinfo));
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
	this->relMap.insert(std :: pair<char *, relInfo>(rel_name, newRel));
}

void Statistics :: AddAtt(char *rel_name, char *att_name, int num_distincts)
{
	auto iter = relMap.find(rel_name);
	if(!strcmp(iter->first, rel_name))
		//relation DNE
		return;

	//NOTE
	//If num_distincts == -1, this means all the values are distinct, and
	//hence it turns out to be the total number of tuples
	//So this attribute is most probably a key and hence must be on the left
	//side of the join
	if (num_distincts == -1)
		num_distincts = iter->second.numTuples;

	iter->second.attrs.insert(std :: pair<char *, int>(att_name,
								num_distincts));
}

void Statistics :: CopyRel(char *oldName, char *newName)
{
	auto iter = relMap.find(oldName);
	if(!strcmp(iter->first, oldName))
		//relation DNE
		return;

	relInfo newRel;

	newRel.numTuples = iter->second.numTuples;
	newRel.numRel = iter->second.numRel;

	for (auto& iter_1: iter->second.attrs){
		char *newAttrs = new char[200];
		//NOTE
		//We used c_str here to convert std :: string to char *
		//but now since we already have char * as type, iter_1.first
		//is sufficient
		//PS. "%s" type specifier requires char * not std :: string
		sprintf(newAttrs, "%s.%s", newName, iter_1.first);
		newRel.attrs.insert(std :: pair<char *,
					int>(newAttrs, iter_1.second));
	}

	relMap.insert(std :: pair<char *, relInfo>(newName, newRel));
}

FILE *Statistics :: f_handle(char *fname, const char *perm)
{
	FILE *f=NULL;
	struct stat buf;
	int ret=stat(fname, &buf);
	int flag=1;
	if(!ret && !strcmp(perm, "w")) {
		std :: cerr << "File " << fname << " exists"
			<< " and is being over written!\n";
	} else if(ret && !strcmp(perm, "r")) {
		std :: cerr << "File " << fname << " doesnt exist, "
			<< "creating a new one!\n";
		if((f=this->f_handle(fname, "w"))==NULL)
			_exit(-1);
		flag=0;
	}

	if(flag) {
		if((f=fopen(fname, perm))==NULL) {
			std :: cerr << "Error in opening file "
				<< fname << std :: endl;
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
	int flag=-1;
	while(!feof(f)) {
		getline(&line, &n, f);
		if(flag==0 || !strcmp(line, "R_BEGIN")) {


		} else if(!strcmp(line, "A_BEGIN")) {

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
		fprintf(f, "R_BEGIN\n%s\n", i.first);
		fprintf(f, "%d\n%d\n", i.second.numTuples, i.second.numRel);
		for(auto& j: i.second.attrs) {
			fprintf(f, "A_BEGIN\n%s\n", j.first);
			fprintf(f, "%d\n", j.second);
			fprintf(f, "A_END\n");
		}

		fprintf(f, "R_END\n");
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
