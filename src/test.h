#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "fs/dbfile.h"
#include "fs/record.h"
using namespace std;

extern "C" {
	int yyparse(void);   // defined in y.tab.c
}

extern struct AndList *final;

class relation {

private:
	const char *rname;
	const char *prefix;
	char rpath[100];
	Schema *rschema;
public:
	relation (const char *_name, Schema *_schema, const char *_prefix) :
		rname (_name), rschema (_schema), prefix (_prefix) {
			sprintf (rpath, "%s%s.bin", prefix, rname);
		}
	const char* name () { return rname; }
	const char* path () { return rpath; }
	Schema* schema () { return rschema;}
	void info () {
		cout << " relation info\n";
		cout << "\t name: " << name () << endl;
		cout << "\t path: " << path () << endl;
	}

	void get_cnf (CNF &cnf_pred, Record &literal) {
		cout << " Enter CNF predicate (when done press ctrl-D):\n\t";
		if (yyparse() != 0) {
			std::cout << "Can't parse your CNF.\n";
			exit (1);
		}
		cnf_pred.GrowFromParseTree (final, schema (), literal); // constructs CNF predicate
	}
};

const char *supplier = "supplier";
const char *partsupp = "partsupp";
const char *part = "part";
const char *nation = "nation";
const char *customer = "customer";
const char *orders = "orders";
const char *region = "region";
const char *lineitem = "lineitem";

relation *s, *p, *ps, *n, *li, *r, *o, *c;
Schema *S_s, *S_ps, *S_p, *S_n, *S_li, *S_r, *S_o, *S_c;

void setup (const char *catalog_path, const char *dbfile_dir, const char *tpch_dir) {
	cout << " \n** IMPORTANT: MAKE SURE THE INFORMATION BELOW IS CORRECT **\n";
	cout << " catalog location: \t" << catalog_path << endl;
	cout << " tpch files dir: \t" << tpch_dir << endl;
	cout << " heap files dir: \t" << dbfile_dir << endl;
	cout << " \n\n";

	//curbing memory leaks
	S_s=new Schema(catalog_path, supplier);
	S_ps=new Schema(catalog_path, partsupp);
	S_p=new Schema(catalog_path, part);
	S_n=new Schema(catalog_path, nation);
	S_li=new Schema(catalog_path, lineitem);
	S_r=new Schema(catalog_path, region);
	S_o=new Schema(catalog_path, orders);
	S_c=new Schema(catalog_path, customer);

	s = new relation (supplier, S_s, dbfile_dir);
	ps = new relation (partsupp, S_ps, dbfile_dir);
	p = new relation (part, S_p, dbfile_dir);
	n = new relation (nation, S_n, dbfile_dir);
	li = new relation (lineitem, S_li, dbfile_dir);
	r = new relation (region, S_r, dbfile_dir);
	o = new relation (orders, S_o, dbfile_dir);
	c = new relation (customer, S_c, dbfile_dir);
}

void cleanup () {
	std :: cout << "Calling cleanup!" << std :: endl;
	delete s, p, ps, n, li, r, o, c;
	delete S_s, S_ps, S_p, S_n, S_li, S_r, S_o, S_c;
}

#endif
