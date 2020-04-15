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

int Statistics :: get_rels(vector<unordered_map<string, relInfo> ::
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

void Statistics :: get_attrs(vector<unordered_map<string, int> ::
			iterator> &vec_att,
			vector<unordered_map<string, relInfo> ::
			iterator> &vec_rel,
			vector<unordered_map<string, relInfo> ::
			iterator> &vec_rel2,
			char *value)
{
	//this function makes sure that the nth attribute in vec_att corresponds
	//to nth relation in vec_rel2
	char *r_ptr=strtok(value, ".");
	char *a_ptr=strtok(NULL, ".");
	unordered_map<string, int> :: iterator att;
	if(a_ptr!=NULL) {
		att=this->attMap.find(string(value));
		if(att==this->attMap.end())
			return;
		else
			vec_att.push_back(att);
	} else {
		int init_size=vec_att.size();
		for(auto i: vec_rel) {
			string a_name=i->first+"."+string(value);
			att=this->attMap.find(a_name);
			if(att!=this->attMap.end()) {
				vec_att.push_back(att);
				vec_rel2.push_back(i);
				break;
			}
		}
	}
}

void Statistics :: join_op(double *res,
			vector<unordered_map<string, relInfo> ::
			iterator> &vec_rel,
			vector<unordered_map<string, int> ::
			iterator> &vec_att, int apply)
{
	double tuples=(((double)vec_rel[0]->second.numTuples)*
			((double)vec_rel[1]->second.numTuples))/
			(double)max(vec_att[0]->second, vec_att[1]->second);
	if(apply) {
		vec_rel[0]->second.joins.insert(vec_rel[1]->first);
		vec_rel[1]->second.joins.insert(vec_rel[0]->first);
		vec_rel[0]->second.numTuples=(int)tuples;
		vec_rel[1]->second.numTuples=(int)tuples;
	}

	*res+=tuples;
}

void Statistics :: sel_op(ComparisonOp *op, double *res,
			vector<unordered_map<string, relInfo> ::
			iterator> &vec_rel,
			vector<unordered_map<string, int> ::
			iterator> &vec_att)

{
	*res+=(double)vec_rel[0]->second.numTuples/3;
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
		vector<unordered_map<string, relInfo> :: iterator> vec1, vec2;
		if(!this->get_rels(vec1, rel_names, n))
			return 0;
		vector<unordered_map<string, int> :: iterator> vec_att;
		if(op->left->code==3)
			this->get_attrs(vec_att, vec1, vec2, op->left->value);
		if(op->right->code==3)
			this->get_attrs(vec_att, vec1, vec2, op->right->value);
		if(vec_att.size()==2)
			this->join_op(res, vec2, vec_att, apply);
		else
			this->sel_op(op, res, vec2, vec_att);
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
