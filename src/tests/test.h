#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "mem/pipe.h"
#include "fs/dbfile.h"
#include "fs/record.h"

using namespace std;

// make sure that the information below is correct

char *catalog_path = "db/catalog";
char *tpch_dir ="tpch-dbgen/"; // dir where dbgen tpch files (extension *.tbl) can be found
char *dbfile_dir = "bin/";


extern "C" {
	int yyparse(void);   // defined in y.tab.c
}

extern struct AndList *final;

typedef struct {
	Pipe *pipe;
	OrderMaker *order;
	bool print;
	bool write;
}testutil;

class relation {

private:
	char *rname;
	char *prefix;
	char rpath[100];
	Schema *rschema;
public:
	relation (char *_name, Schema *_schema, char *_prefix) :
		rname (_name), rschema (_schema), prefix (_prefix) {
			sprintf (rpath, "%s%s.bin", prefix, rname);
		}
	char* name () { return rname; }
	char* path () { return rpath; }
	Schema* schema () { return rschema;}
	void info () {
		cout << " relation info\n";
		cout << "\t name: " << name () << endl;
		cout << "\t path: " << path () << endl;
	}

	void get_cnf (CNF &cnf_pred, Record &literal) {
		cout << " Enter CNF predicate (when done press ctrl-D):\n\t";
		if (yyparse() != 0) {
			cout << "Can't parse your CNF.\n";
			exit (1);
		}
		cnf_pred.GrowFromParseTree (final, schema (), literal); // constructs CNF predicate
	}
	void get_sort_order (OrderMaker &sortorder) {
		cout << "\n specify sort ordering (when done press ctrl-D):\n\t ";
		if (yyparse() != 0) {
			cout << "Can't parse your sort CNF.\n";
			exit (1);
		}
		cout << " \n";
		Record literal;
		CNF sort_pred;
		sort_pred.GrowFromParseTree (final, schema (), literal); // constructs CNF predicate
		OrderMaker dummy;
		sort_pred.GetSortOrders (sortorder, dummy);
	}
};

char *supplier = "supplier";
char *partsupp = "partsupp";
char *part = "part";
char *nation = "nation";
char *customer = "customer";
char *orders = "orders";
char *region = "region";
char *lineitem = "lineitem";

relation **rel;
Schema **sch;

void setup () {
	cout << " \n** IMPORTANT: MAKE SURE THE INFORMATION BELOW IS CORRECT **\n";
	cout << " catalog location: \t" << catalog_path << endl;
	cout << " tpch files dir: \t" << tpch_dir << endl;
	cout << " heap files dir: \t" << dbfile_dir << endl;
	cout << " \n\n";

	sch=new Schema *[8];
	rel=new relation *[8];

	//curbing memory leaks
	sch[0]=new Schema(catalog_path, nation);
	sch[1]=new Schema(catalog_path, region);
	sch[2]=new Schema(catalog_path, customer);
	sch[3]=new Schema(catalog_path, part);
	sch[4]=new Schema(catalog_path, partsupp);
	sch[5]=new Schema(catalog_path, supplier);
	sch[6]=new Schema(catalog_path, orders);
	sch[7]=new Schema(catalog_path, lineitem);

	rel[0]= new relation (nation, sch[0], dbfile_dir);
	rel[1]= new relation (region, sch[1], dbfile_dir);
	rel[2]= new relation (customer, sch[2], dbfile_dir);
	rel[3]= new relation (part, sch[3], dbfile_dir);
	rel[4]= new relation (partsupp, sch[4], dbfile_dir);
	rel[5]= new relation (supplier, sch[5], dbfile_dir);
	rel[6]= new relation (orders, sch[6], dbfile_dir);
	rel[7]= new relation (lineitem, sch[7], dbfile_dir);
}

void cleanup () {
	//NOTE
	//changed to the array layout for both memory and relation as delete
	//cannot be called with multiple arguments like
	//delete a, b, c;
	//this left a lot of memory leaks, fixed test.cc as well.
	std :: cout << "Calling cleanup!" << std :: endl;
	for(int i=0; i<8; i++) {
		delete rel[i];
		delete sch[i];
	}
	delete[] rel;
	delete[] sch;
}

#endif
