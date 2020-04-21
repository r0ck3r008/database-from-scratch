#include<iostream>
#include<algorithm>
#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<math.h>

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
	// Since the rels array has the names of relations exactly like the
	// operation would need to access and we are assuming atomic operations
	// in per each AndList->left, we can just assume the relations are right
	// here and skip the set checking altogether. This will speed up lookup
	// by alot overall
	struct Operand *operand=cop->left;
	int num=0;
	while(num<=n && operand->code==NAME) {
		pair<unordered_map<string, relInfo> :: iterator,
			unordered_map<string, int> :: iterator> p;
		char name[64];
		sprintf(name, "%s", operand->value);
		char *_r_name=strtok(name, "."), *_a_name=strtok(NULL, ".");
		string r_name(_r_name), a_name(_a_name);
		auto itr1=this->relMap.find(r_name);
		if(itr1==this->relMap.end())
			return 0;
		auto itr2=itr1->second.attMap.find(a_name);
		if(itr2==itr1->second.attMap.end())
			return 0;

		p.first=itr1; p.second=itr2;
		vec_rels.push_back(p);
		num++;
		operand=cop->right;
	}
	return 1;
}

double Statistics :: join_op(struct ComparisonOp *,
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

	return tuples;
}

double Statistics :: sel_op(struct ComparisonOp *cop,
			vector<pair<unordered_map<string, relInfo> :: iterator,
			unordered_map<string, int> :: iterator>> &vec)

{
	double res;
	if(cop->code==EQUALS) {
		res=((double)vec[0].first->second.numTuples)/
			((double)vec[0].second->second);
	} else {
		res=((double)vec[0].first->second.numTuples)/3;
	}

	return res;
}

double Statistics :: traverse(AndList *a_list, OrList *o_list, char **rel_names,
				int n, int apply)
{
	double res=0.0;
	if(o_list==NULL && a_list->left!=NULL) {
		//Move down from AND to OR
		res=this->traverse(a_list, a_list->left, rel_names, n, apply);
	} else if(o_list!=NULL) {
		//Execute OR
		vector<pair<unordered_map<string, relInfo> :: iterator,
			unordered_map<string, int> :: iterator>> vec_rels;
		if(!this->get_rels(vec_rels, o_list->left, rel_names, n)) {
			cerr << "Error in fetching the attributes!\n";
			return 0.0;
		}
		if(a_list->is_join)
			res=this->join_op(o_list->left, vec_rels, apply);
		else
			res=this->sel_op(o_list->left, vec_rels);
	}
	if(o_list!=NULL && o_list->rightOr!=NULL)  {
		//Move right from OR to OR and do a+b-a*b
		double ans=this->traverse(a_list, o_list->rightOr, rel_names,
								n, apply);
		res=fabs(res + ans - (res * ans));
	} else if(o_list==NULL && a_list->rightAnd!=NULL) {
		//Move right from AND to AND and multiply the AND results
		//together
		double ans=this->traverse(a_list->rightAnd, NULL, rel_names, n,
									apply);
		res*=ans;
	}

	return res;
}

// Either its an Or node or an And node
// If its an And node, it must receive its total Or answer from its subordinate
