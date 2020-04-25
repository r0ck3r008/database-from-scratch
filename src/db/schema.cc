#include <iostream>
#include <string.h>

#include "schema.h"

using namespace std;

Schema :: Schema(){}
Schema :: ~Schema(){}

int Schema :: Find (char *attName)
{
	string rname, aname;
	splice(attName, &rname, &aname);
	auto itr=this->attMap.find(aname);
	if(itr==this->attMap.end())
		return -1;
	else
		return 1;
}

Type Schema :: FindType (char *attName)
{
	string rname, aname;
	splice(attName, &rname, &aname);
	auto itr=this->attMap.find(aname);
	if(itr==this->attMap.end())
		return Int;
	else
		return itr->second.myType;
}

void Schema :: Print()
{
	for(auto &i: this->attMap)
		cout << i.first.c_str() << " " << i.second.myType << endl;
}

void splice(char *_aname, string *rname, string *aname)
{
	char name[64];
	sprintf(name, "%s", _aname);
	*rname=string(strtok(name, "."));
	*aname=string(strtok(NULL, "."));
}
