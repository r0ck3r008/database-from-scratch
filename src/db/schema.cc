#include<stdlib.h>
#include<string.h>

#include"schema.h"

using namespace std;

void splice(char *attName, char *placeholder)
{
	char name[64];
	sprintf(name, "%s", attName);
	char *tbl=strtok(name, ".");
	if(!strcmp(tbl, attName))
		sprintf(placeholder, "%s", attName);
	else
		sprintf(placeholder, "%s", strtok(NULL, "."));
}

Attribute :: Attribute()
{
	this->key=0;
	this->name=NULL;
	this->n_dis=0;
}

Attribute :: Attribute(char *name, Type type)
{
	this->name=strdup(name);
	this->key=0;
	this->n_dis=0;
	this->myType=type;
}

Attribute :: ~Attribute()
{
	if(this->name!=NULL)
		free(this->name);
}

const Attribute &Attribute :: operator=(const Attribute &in)
{
	this->name=strdup(in.name);
	this->myType=in.myType;
	this->n_dis=n_dis;

	return *(this);
}

Schema :: Schema()
{
	this->numAtts=0;
	this->myAtts=new Attribute[64];
}

Schema :: Schema(string fname, fType type, int n_tup)
{
	this->fname=fname;
	this->type=type;
	this->n_tup=n_tup;
	this->numAtts=0;
	this->myAtts=new Attribute[64];
}

Schema :: Schema (char *fpath, int num_atts, Attribute *atts)
{
	this->fname = string(fpath);
	numAtts = num_atts;
	this->myAtts=new Attribute[64];
	for (int i = 0; i < numAtts; i++ ) {
		if (atts[i].myType == Int) {
			myAtts[i].myType = Int;
		}
		else if (atts[i].myType == Double) {
			myAtts[i].myType = Double;
		}
		else if (atts[i].myType == String) {
			myAtts[i].myType = String;
		}
		else {
			cout << "Bad attribute type for " << atts[i].myType << "\n";
			delete [] myAtts;
			exit (1);
		}
		myAtts[i].name = strdup (atts[i].name);
	}

}

Schema :: Schema (char *fName, char *relName)
{
	FILE *foo = fopen (fName, "r");

	// this is enough space to hold any tokens
	char space[200];

	fscanf (foo, "%s", space);
	int totscans = 1;

	// see if the file starts with the correct keyword
	if (strcmp (space, "BEGIN")) {
		cout << "Unfortunately, this does not seem to be a schema file.\n";
		exit (1);
	}

	while (1) {

		// check to see if this is the one we want
		fscanf (foo, "%s", space);
		totscans++;
		if (strcmp (space, relName)) {

			// it is not, so suck up everything to past the BEGIN
			while (1) {

				// suck up another token
				if (fscanf (foo, "%s", space) == EOF) {
					cerr << "Could not find the schema for the specified relation.\n";
					exit (1);
				}

				totscans++;
				if (!strcmp (space, "BEGIN")) {
					break;
				}
			}

			// otherwise, got the correct file!!
		} else {
			break;
		}
	}

	// suck in the file name
	fscanf (foo, "%s", space);
	totscans++;
	this->fname= string(space);

	// count the number of attributes specified
	numAtts = 0;
	while (1) {
		fscanf (foo, "%s", space);
		if (!strcmp (space, "END")) {
			break;
		} else {
			fscanf (foo, "%s", space);
			numAtts++;
		}
	}

	// now actually load up the schema
	fclose (foo);
	foo = fopen (fName, "r");

	// go past any un-needed info
	for (int i = 0; i < totscans; i++) {
		fscanf (foo, "%s", space);
	}

	// and load up the schema
	this->myAtts=new Attribute[64];
	for (int i = 0; i < numAtts; i++ ) {

		// read in the attribute name
		fscanf (foo, "%s", space);
		myAtts[i].name = strdup (space);

		// read in the attribute type
		fscanf (foo, "%s", space);
		if (!strcmp (space, "Int")) {
			myAtts[i].myType = Int;
		} else if (!strcmp (space, "Double")) {
			myAtts[i].myType = Double;
		} else if (!strcmp (space, "String")) {
			myAtts[i].myType = String;
		} else {
			cout << "Bad attribute type for " << myAtts[i].name << "\n";
			exit (1);
		}
	}

	fclose (foo);
}

Schema :: ~Schema ()
{
	delete [] myAtts;
}

Schema &Schema :: operator+(Schema &in)
{
	for(int i=0; i<in.numAtts; i++)
		this->addAtt(in.myAtts[i].name, in.myAtts[i].myType,
						in.myAtts[i].n_dis, 0);

	return *(this);
}

void Schema :: addAtt(char *aname, Type type, int n_dis, int key)
{
	if(numAtts==64) {
		cerr << "No more attributes can be added!\n";
		return;
	}
	myAtts[numAtts].name=strdup(aname);
	myAtts[numAtts].myType=type;
	myAtts[numAtts].n_dis=n_dis;
	myAtts[numAtts].key=key;
	this->attMap.insert(pair<string, Attribute *>(string(aname),
							&(myAtts[numAtts])));
	numAtts++;
}

int Schema :: Find (char *attName)
{
	char placeholder[64];
	splice(attName, placeholder);
	for (int i = 0; i < numAtts; i++) {
		if (!strcmp (placeholder, myAtts[i].name)) {
			return i;
		}
	}

	// if we made it here, the attribute was not found
	return -1;
}

Type Schema :: FindType (char *attName)
{
	char placeholder[64];
	splice(attName, placeholder);
	for (int i = 0; i < numAtts; i++) {
		if (!strcmp (placeholder, myAtts[i].name)) {
			return myAtts[i].myType;
		}
	}

	// if we made it here, the attribute was not found
	return Int;
}

int Schema :: GetNumAtts ()
{
	return numAtts;
}

Attribute *Schema :: GetAtts ()
{
	return myAtts;
}

void Schema :: Print()
{
	for(int i=0; i<this->numAtts; i++) {
		cout << this->myAtts[i].name << " ";
		if(this->myAtts[i].myType == Int)
			cout << "Int\n";
		else if(this->myAtts[i].myType == String)
			cout << "String\n";
		else if(this->myAtts[i].myType == Double)
			cout << "Double\n";
	}
}
