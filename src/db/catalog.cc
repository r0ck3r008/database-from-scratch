#include<iostream>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

#include"catalog.h"

using namespace std;

void Catalog :: addRel(char *_rname, char *_fname, fType type, int n_tup)
{
	string rname(_rname), fname(_fname);
	auto itr=this->rels.find(rname);
	if(itr==this->rels.end()) {
		cerr << "This relation already exists!\n";
		return;
	}

	Schema rinfo;
	rinfo.n_tup=n_tup;
	rinfo.type=type;
	rinfo.fname=fname;
	this->rels.insert(pair<string, Schema>(rname, rinfo));
}

void Catalog :: addAtt(char *_rname, char *_aname, int n_dis, Type type)
{
	string rname(_rname), aname(_aname);
	auto itr1=this->rels.find(rname);
	if(itr1==this->rels.end()) {
		cerr << "This relation does not exit!\n";
		return;
	}

	auto itr2=itr1->second.attMap.find(aname);
	if(itr2!=itr1->second.attMap.end()) {
		cerr << "This attribute already exists!\n";
		return;
	}

	Attribute ainfo;
	ainfo.n_dis=(n_dis==-1) ? (itr1->second.n_tup) : (n_dis);
	ainfo.myType=type;
	itr1->second.attMap.insert(pair<string, Attribute>(aname, ainfo));
}

Schema *Catalog :: snap(char *_rname)
{
	string rname(_rname);
	auto itr=this->rels.find(rname);
	if(itr==this->rels.end()) {
		cerr << "Error in finding relation: " << rname << endl;
		return NULL;
	}

	Schema *rinfo=new Schema(itr->second);
	return rinfo;
}

void Catalog :: write(char *fname)
{
	FILE *f=NULL;
	if((f=f_handle(fname, "w"))==NULL)
		return;

	for(auto &i: this->rels) {
		fprintf(f, "R_BEGIN:%s:%d:%d:%s\n", i.first.c_str(),
			i.second.n_tup, i.second.type,
						i.second.fname.c_str());
		for(auto &j: i.second.attMap) {
			fprintf(f, "A_BEGIN:%s:%d:%d\n", j.first.c_str(),
				j.second.myType, j.second.n_dis);
		}
	}

	fclose(f);
}

void Catalog :: read(char *fname)
{
	FILE *f=NULL;
	if((f=f_handle(fname, "r"))==NULL)
		return;

	char *line_tmp=NULL;
	size_t n=0;
	int begin=1;
	Schema *curr_rinfo=new Schema;
	string curr_rname;
	while(!feof(f)) {
		int stat=getline(&line_tmp, &n, f);
		char line[strlen(line_tmp)];
		sprintf(line, "%s", line_tmp);
		if(stat==-1)
			break;

		char *part=strtok(line, ":");
		if(!strcmp(part, "R_BEGIN")) {
			if(!begin) {
				this->rels.insert(pair<string, Schema>
						(curr_rname, *(curr_rinfo)));
				delete curr_rinfo;
				curr_rinfo=new Schema;
			} else {
				begin=1;
			}
			curr_rname=string(strtok(NULL, ":"));
			curr_rinfo->n_tup=strtol(strtok(NULL, ":"), NULL, 10);
			curr_rinfo->type=(fType)strtol(strtok(NULL, ":"),
								NULL, 10);
			curr_rinfo->fname=string(strtok(NULL, ":"));
		} else if(!strcmp(part, "A_BEGIN")) {
			Attribute ainfo;
			string aname(strtok(NULL, ":"));
			ainfo.myType=(Type)strtol(strtok(NULL, ":"), NULL, 10);
			ainfo.n_dis=strtol(strtok(NULL, ":"), NULL, 10);
			curr_rinfo->attMap.insert(pair<string, Attribute>
							(aname, ainfo));
		}

		free(line_tmp);
		line_tmp=NULL;
	}

	free(line_tmp);
	fclose(f);
}

FILE *f_handle(char *fname, const char *perm)
{
	struct stat buf;
	int ret=stat(fname, &buf);
	FILE *f=NULL;
	int flag=1;

	if(!ret && !strcmp("w", perm)) {
		cerr << "File: " << fname << " already exists, overwriting!\n";
	} else if(ret==-1 && !strcmp("r", perm)) {
		cerr << "File: " << fname << " does not exist, creating one!\n";
		f=f_handle(fname, "w");
		flag=0;
	}

	if(flag) {
		if((f=fopen(fname, perm))==NULL) {
			cerr << "Error in Opening the file: " << fname << "!\n";
		}
	}

	return f;
}
