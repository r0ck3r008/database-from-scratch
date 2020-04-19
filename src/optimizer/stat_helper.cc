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

int Statistics :: get_rels(vector<unordered_map<string, relInfo> :: iterator>
void Statistics :: fetch_att_name(char *name, string *r_name, string *a_name)
{
	char _a_name[64];
	sprintf(_a_name, "%s", name);
	char *part=strtok(_a_name, ".");
	if(!strcmp(part, name)) {
		*a_name=string(_a_name);
		*r_name="";
	} else {
		*r_name=string(part);
		*a_name=string(strtok(NULL, "."));
	}
}

			&vec_rels, struct ComparisonOp *cop, char **rel_names,
			int n)
{
	//This function enforces the set rules within rel_names are returns a
	//error code=0 if they are not being followed. This simultaniosly finds
	//the matching relations as well
	struct Operand *op=cop->left;
	set<string> left, right;
	while(op->code==3) {
		string a_name=string(op->value);
		for(int i=0; i<n; i++) {
			string r_name=string(rel_names[i]);
			auto itr1=this->relMap.find(r_name);
			auto itr2=itr1->second.attMap.find(a_name);
			if(itr2==itr1->second.attMap.end()) {
				right.insert(itr1->first);
			} else {
				vec_rels.push_back(itr1);
				left.insert(itr1->second.joins.begin(),
						itr1->second.joins.end());
			}
		}
		op=cop->right;
	}

	if(left==right)
		return 1;
	else
		return 0;
}

void Statistics :: join_op(struct ComparisonOp *cop, double *res,
			vector<unordered_map<string, relInfo> ::
			iterator> &vec_rel, int apply)
{
	auto att1=vec_rel[0]->second.attMap.find(string(cop->left->value));
	auto att2=vec_rel[1]->second.attMap.find(string(cop->right->value));
	double tuples=(((double)vec_rel[0]->second.numTuples)*
			((double)vec_rel[1]->second.numTuples))/
			(double)max(att1->second, att2->second);
	if(apply) {
		vec_rel[0]->second.joins.insert(vec_rel[1]->first);
		vec_rel[1]->second.joins.insert(vec_rel[0]->first);
		vec_rel[0]->second.numTuples=(int)tuples;
		vec_rel[1]->second.numTuples=(int)tuples;
	}

	*res+=tuples;
}

void Statistics :: sel_op(struct ComparisonOp *cop, double *res,
			vector<unordered_map<string, relInfo> :: iterator> &vec)
{
	//TODO
	////Fix estimation function
	*res+=(double)vec[0]->second.numTuples/3;
}

int Statistics :: traverse(AndList *a_list, OrList *o_list, double *res,
				char **rel_names, int n, int apply)
{
	if(o_list==NULL && a_list->left!=NULL) {
		//Move down from AND to OR
		if(!this->traverse(a_list, a_list->left, res, rel_names, n,
									apply))
			return 0;
	} else if(o_list!=NULL) {
		//Execute OR
		vector<unordered_map<string, relInfo> :: iterator> vec_rels;
		if(!this->get_rels(vec_rels, o_list->left, rel_names, n))
			return 0;
		if(vec_rels.size()==2)
			this->join_op(o_list->left, res, vec_rels, apply);
		else
			this->sel_op(o_list->left, res, vec_rels);
	}
	if(o_list!=NULL && o_list->rightOr!=NULL) {
		//Move right from OR to OR
		if(!this->traverse(a_list, o_list->rightOr, res, rel_names,
									n, apply))
			return 0;
	}
	if(a_list->rightAnd!=NULL) {
		//Move right from AND to AND
		if(!this->traverse(a_list->rightAnd, NULL, res, rel_names, n,
									apply))
			return 0;
	}

	return 1;
}
