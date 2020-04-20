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

int Statistics :: get_rels(vector<pair<unordered_map<string, relInfo> ::
			iterator, unordered_map<string, int> :: iterator>>
			&vec_rels, struct ComparisonOp *cop, char **rel_names,
			int n)
{
	//This function enforces the set rules within rel_names are returns a
	//error code=0 if they are not being followed. This simultaniosly finds
	//the matching relations as well
	struct Operand *op=cop->left;
	set<string> left, right;
	int runs=0;
	while(runs<2 && op->code==NAME) {
		for(int i=0; i<n; i++) {
			string a_name="", r_name="";
			this->fetch_att_name(op->value, &r_name, &a_name);
			if(r_name.compare("")==0)
				r_name=string(rel_names[i]);
			auto itr1=this->relMap.find(r_name);
			auto itr2=itr1->second.attMap.find(a_name);
			if(itr2!=itr1->second.attMap.end()){
				pair<unordered_map<string, relInfo> :: iterator,
				unordered_map<string, int> :: iterator> p(itr1,
									itr2);
				vec_rels.push_back(p);
				left.insert(itr1->second.joins.begin(),
						itr1->second.joins.end());
				break;
			}
		}
		op=cop->right;
		runs++;
	}
	for(int i=0; i<n; i++)
		right.insert(string(rel_names[i]));

	if(runs==2 && left!=right)
		return 0;
	else
		return 1;
}

void Statistics :: join_op(struct ComparisonOp *, double *res,
		vector<pair<unordered_map<string, relInfo> :: iterator,
		unordered_map<string, int> :: iterator>> &vec, int apply)
{
	double tuples=(((double)vec[0].first->second.numTuples)*
			((double)vec[1].first->second.numTuples))/
			(double)max(vec[0].second->second,
					vec[1].second->second);
	if(apply) {
		vec[0].first->second.joins.insert(vec[1].first->first);
		vec[1].first->second.joins.insert(vec[0].first->first);
		vec[0].first->second.numTuples=(int)tuples;
		vec[1].first->second.numTuples=(int)tuples;
	}

	*res+=tuples;
}

void Statistics :: sel_op(struct ComparisonOp *cop, double *res,
			vector<pair<unordered_map<string, relInfo> :: iterator,
			unordered_map<string, int> :: iterator>> &vec)

{
	if(cop->code==EQUALS) {
		*res+=((double)vec[0].first->second.numTuples)/
			((double)vec[0].second->second);
	} else {
		*res+=((double)vec[0].first->second.numTuples)/3;
	}
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
		vector<pair<unordered_map<string, relInfo> :: iterator,
			unordered_map<string, int> :: iterator>> vec_rels;
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
