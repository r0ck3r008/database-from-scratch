#include<vector>
#include<algorithm>
#include<string.h>

#include"statistics.h"

using namespace std;

Statistics :: Statistics(Catalog *c)
{
	this->c=c;
}
Statistics :: ~Statistics()
{
	for(auto &i: this->relMap)
		delete i.second;
}

void Statistics :: addRel(char *_rname, char *_alias)
{
	Schema *sch=this->c->snap(_rname);
	if(sch==NULL)
		return;

	string rname;
	if(_alias==NULL)
		rname=string(_rname);
	else
		rname=string(_alias);

	this->relMap.insert(pair<string, Schema *>(rname, sch));
}

double Statistics :: operate(struct OrList *olist)
{
	struct ComparisonOp *cop=olist->left;
	struct Operand *op=cop->left;
	vector<pair<Schema *, Attribute *>> vec;
	while(vec.size()<3 && op->code==NAME) {
		Schema *sch;
		Attribute *att;
		pair<Schema *, Attribute *> p;
		this->get_rel(op->value, &sch, &att);
		p.first=sch;
		p.second=att;
		vec.push_back(p);
		op=cop->right;
	}

	double res=0.0;
	if(vec.size()==2) {
		//join
		res=(((double)vec[0].first->n_tup)*
			((double)vec[1].first->n_tup))/
			(double)max((vec[0].second->n_dis),
				vec[1].second->n_dis);
	} else {
		if(op->code==EQUALS) {
			res=((double)vec[0].first->n_tup)/
				((double)vec[0].second->n_dis);
		} else {
			res=((double)vec[0].first->n_tup)/3;
		}
	}

	return res;
}

void Statistics :: get_rel(char *_aname, Schema **sch, Attribute **attr)
{
	string rname, aname;
	char name[64];
	sprintf(name, "%s", _aname);
	char *rel=strtok(name, ".");
	if(!strcmp(rel, _aname)) {
		cerr << "Attribute name in wrong format!\n";
		return;
	}

	rname=string(rel);
	aname=string(strtok(NULL, "."));
	auto itr=this->relMap.find(rname);
	if(itr==this->relMap.end()) {
		cerr << "Error in finding the relation!\n";
		return;
	}

	auto itr2=itr->second->attMap.find(aname);
	if(itr2==itr->second->attMap.end()) {
		cerr << "Error in finding the attribute!\n";
		return;
	}

	*attr=itr2->second;
	*sch=itr->second;
}

double Statistics :: traverse(struct AndList *alist, struct OrList *olist)
{
	double curr_res=0.0;
	if(olist==NULL && alist->left!=NULL) {
		curr_res=this->traverse(alist, alist->left);
	} else if(olist!=NULL) {
		//operate
		curr_res=this->operate(olist);
	}

	if(olist!=NULL && olist->rightOr!=NULL) {
		double res=this->traverse(alist, olist->rightOr);
		curr_res=curr_res+res-(curr_res*res);
	} else if(olist==NULL && alist->rightAnd!=NULL) {
		curr_res*=this->traverse(alist->rightAnd, NULL);
	}

	return curr_res;
}

double Statistics :: Estimate(struct AndList *a_list)
{
	return (this->traverse(a_list, NULL));
}
