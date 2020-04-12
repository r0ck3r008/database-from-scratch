#include<iostream>
#include<algorithm>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

#include"statistics.h"

using namespace std;

FILE *Statistics :: f_handle(char *fname, const char *perm)
{
	FILE *f=NULL;
	int flag=1;
	struct stat buf;
	int res=stat(fname, &buf);
	if(res==0 && !strcmp(perm, "w")) {
		std :: cerr << "Overwriting file " << fname
			<< std :: endl;
	} else if(res<0 && !strcmp(perm, "r")) {
		std :: cerr << "File " << fname << " doesnt exist!\n";
		f=this->f_handle(fname, std :: string("w").c_str());
		flag=0;
	}

	if(flag) {
		if((f=fopen(fname, perm))==NULL) {
			std :: cerr << "Error in opening file " << fname
				<< " in " << perm << " mode!\n";
			return NULL;
		}
	}

	return f;
}

double Statistics :: join_op(ComparisonOp *op, int apply)
{
	double curr_res=0.0;
	string n_att1(op->left->value);
	string n_att2(op->right->value);
	auto att1=this->attMap.find(n_att1);
	auto att2=this->attMap.find(n_att2);
	auto rel1=this->relMap.find(att1->second.relName);
	auto rel2=this->relMap.find(att2->second.relName);

	curr_res=((rel1->second.numTuples)*(rel2->second.numTuples))/
			max(att1->second.n_distinct, att2->second.n_distinct);

	if(apply) {
		//update relation
		relInfo r_info;
		r_info.numTuples=(int)curr_res;
		r_info.relCount=(rel1->second.relCount)+(rel2->second.relCount);
		string rel_name=rel1->first + "+" + rel2->first;
		this->relMap.erase(rel1->first);
		this->relMap.erase(rel2->first);
		this->relMap.insert(pair<string, relInfo>(rel_name, r_info));
	}

	return curr_res;
}

double Statistics :: sel_op(ComparisonOp *op)
{
	double curr_res=0.0;

	return curr_res;
}

double Statistics :: traverse(AndList *a_list, OrList *o_list, int apply)
{
	double curr_res=0.0;
	if(o_list==NULL && a_list->left!=NULL) {
		//Move down from AND to OR
		curr_res+=this->traverse(a_list, a_list->left, apply);
	} else if(o_list!=NULL) {
		struct ComparisonOp *op=o_list->left;
		//Execute OR
		if(op->code==3 && op->left->code==4 && op->right->code==4)
			curr_res+=this->join_op(op, apply);
		else
			curr_res+=this->sel_op(op);
	}

	if(o_list->rightOr!=NULL)
		//Move right from OR to OR
		curr_res+=this->traverse(a_list, o_list->rightOr, apply);

	if(a_list->rightAnd!=NULL)
		//Move right from AND to AND
		curr_res+=this->traverse(a_list->rightAnd, NULL, apply);

	return curr_res;
}
