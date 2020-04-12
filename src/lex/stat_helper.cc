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

double Statistics :: traverse(AndList *a_list, OrList *o_list, int apply)
{
	double curr_res=0.0;
	if(o_list==NULL && a_list->left!=NULL)
		//Move down from AND to OR
		curr_res+=this->traverse(a_list, a_list->left, apply);
	else if(o_list!=NULL)
		//Execute OR
		curr_res+=this->operate(o_list->left, apply);

	if(o_list->rightOr!=NULL)
		//Move right from OR to OR
		curr_res+=this->traverse(a_list, o_list->rightOr, apply);

	if(a_list->rightAnd!=NULL)
		//Move right from AND to AND
		curr_res+=this->traverse(a_list->rightAnd, NULL, apply);

	return curr_res;
}
