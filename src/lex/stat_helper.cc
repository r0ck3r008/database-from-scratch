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

int Statistics :: get_relations(vector<unordered_map<string, relInfo> ::
				iterator> &vec, char **rel_names, int n)
{
	set<string> pres, chkr;
	for(int i=0; i<n; i++) {
		string r_name(rel_names[i]);
		pres.insert(r_name);
		auto itr=this->relMap.find(r_name);
		if(itr==this->relMap.end())
			return 0;

		chkr.insert(itr->second.joins.begin(),
				itr->second.joins.end());
		vec.push_back(itr);
	}

	if(pres==chkr)
		return 1;
	else
		return 0;
}

int Statistics :: join_op(ComparisonOp *op, double *res,
				char **rel_names, int n, int apply)
{
	vector<unordered_map<string, relInfo> :: iterator> vec;
	if(!this->get_relations(vec, rel_names, n))
		return 0;

	return 1;
}

int Statistics :: sel_op(ComparisonOp *op, double *res,
					char **rel_names, int n)
{
	vector<unordered_map<string, relInfo> :: iterator> vec;
	if(!this->get_relations(vec, rel_names, n))
		return 0;

	return 1;
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
		struct ComparisonOp *op=o_list->left;
		if(op->code==3 && op->left->code==4 && op->right->code==4) {
			if(!this->join_op(op, res, rel_names, n, apply))
				return 0;
		} else {
			if(!this->sel_op(op, res, rel_names, n))
				return 0;
		}
	}

	if(o_list->rightOr!=NULL) {
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
