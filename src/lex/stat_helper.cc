#include<iostream>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>

#include"statistics.h"

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

void Statistics :: cost_calc(struct ComparisonOp *op, int apply, double *res)
{
	string val1=string(op->left->value);
	string val2=string(op->right->value);
	int join=0;
	auto att1=this->attrs.find(val1);
	auto att2=this->attrs.find(val2);
	auto rel1=this->relMap.find(att1->second.rel_name);
	auto rel2=this->relMap.find(att2->second.rel_name);

	if(apply && join) {
		relInfo relinfo;
		string rel_name=rel1->first + rel2->first;
		//update attributes
		att1->second.rel_name=rel_name;
		att2->second.rel_name=rel_name;
		//update relation
		int max=(rel1->second.numTuples >= rel2->second.numTuples) ?
			(rel1->second.numTuples) : (rel2->second.numTuples);
		int numTuples=((rel1->second.numTuples)*
					(rel2->second.numTuples))/max;
		relinfo.numTuples=numTuples;
		relinfo.numRel=(rel1->second.numRel)+(rel2->second.numRel);
		this->relMap.erase(rel1->first);
		this->relMap.erase(rel2->first);
		this->relMap.insert(pair<string, relInfo>(rel_name, relinfo));
	}
}

void Statistics :: stat_helper(struct AndList *alist, struct OrList *olist,
							int apply, double *res)
{
	if(olist==NULL)
		//this is a AND node moving to another OR node
		this->stat_helper(alist, alist->left, apply, res);
	else


	if(olist!=NULL && olist->rightOr!=NULL)
		//this is an OR node moving to another OR node
		this->stat_helper(alist, olist->rightOr, apply, res);

	if(olist==NULL && alist->rightAnd!=NULL)
		//This is an AND node moving to another AND node
		this->stat_helper(alist->rightAnd, NULL, apply, res);

}
