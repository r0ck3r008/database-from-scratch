// TODO
// 1. instead of combining andlists in process(operation1, operation2), combine
// operation structures instead
// 3. Join operation should be added below select operation only if both of
// those relations have more than 1 select operations
// 4. Append all the attributes that a relation needs during the whole query to
// the tableInfo structure
#include<string.h>
#include<unistd.h>

#include"qp_tree.h"

using namespace std;

void Qptree :: get_attr(char *att_name, pair<string, unordered_map<string,
			tableInfo> :: iterator> &p)
{
	char name[64];
	sprintf(name, "%s", att_name);
	string tbl=string(strtok(att_name, "."));
	string att=string(strtok(NULL, "."));

	auto itr=this->relations.find(tbl);
	if(itr==this->relations.end()) {
		cerr << "Attribute " << att_name << "not found!\n";
		_exit(-1);
	}

	p.first=att;
	p.second=itr;
}
