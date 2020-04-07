#include<iostream>
#include<algorithm>
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
	} else if(ret==-1 && !strcmp(perm, "r")) {
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
	//ugly
	string val1=string(op->left->value);
	string val2=string(op->right->value);
	int flag=0;
	double numTuples;

	auto att1=this->attrs.find(val1);
	auto att2=this->attrs.find(val2);
	if((att1!=this->attrs.end()) && (att2!=this->attrs.end()))
		flag=1;

	if(flag) {
		auto rel1=this->relMap.find(att1->second.rel_name);
		auto rel2=this->relMap.find(att2->second.rel_name);
		double min_tup=min(rel1->second.numTuples,
						rel2->second.numTuples);
		numTuples=(((double)rel1->second.numTuples)*
				((double)rel2->second.numTuples))/min_tup;
		*res+=numTuples;
	} else {
		//terniary operator doest work with iterators, why?
		auto att=att1;
		if(att1==this->attrs.end())
			att=att2;
		auto rel=this->relMap.find(att->second.rel_name);
		if(rel==this->relMap.end())
			return;
		*res+=*res+((double)rel->second.numTuples)/
			((double)(att->second.num_distinct));
	}

	if(apply && flag) {
		relInfo relinfo;
		auto rel1=this->relMap.find(att1->second.rel_name);
		auto rel2=this->relMap.find(att2->second.rel_name);
		string rel_name=rel1->first + "|" + rel2->first;
		//update relation
		relinfo.numTuples=numTuples;
		relinfo.numRel=(rel1->second.numRel)+(rel2->second.numRel);
		this->relMap.insert(pair<string, relInfo>(rel_name, relinfo));
		//update attributes
		//super ugly
		for(auto& i: this->attrs) {
			if(i.second.rel_name.compare(rel1->first)==0) {
				char r_name[64], r1_name[64];
				sprintf(r_name, "%s", rel_name.c_str());
				sprintf(r1_name, "%s", rel1->first.c_str());
				this->CopyAttrs(r1_name, r_name);
			} else if(i.second.rel_name.compare(rel2->first)==0) {
				char r_name[64], r2_name[64];
				sprintf(r_name, "%s", rel_name.c_str());
				sprintf(r2_name, "%s", rel2->first.c_str());
				this->CopyAttrs(r2_name, r_name);
			}
		}
		this->relMap.erase(rel1->first);
		this->relMap.erase(rel2->first);
	}
}

void Statistics :: stat_helper(struct AndList *alist, struct OrList *olist,
							int apply, double *res)
{
	if(olist==NULL)
		//this is a AND node moving to another OR node
		this->stat_helper(alist, alist->left, apply, res);
	else
		this->cost_calc(olist->left, apply, res);


	if(olist!=NULL && olist->rightOr!=NULL)
		//this is an OR node moving to another OR node
		this->stat_helper(alist, olist->rightOr, apply, res);

	if(olist==NULL && alist->rightAnd!=NULL)
		//This is an AND node moving to another AND node
		this->stat_helper(alist->rightAnd, NULL, apply, res);

}
