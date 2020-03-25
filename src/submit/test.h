#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include "function.h"
#include "pipe.h"
#include "dbfile.h"
#include "record.h"

using namespace std;

// test settings file should have the
// catalog_path, dbfile_dir and tpch_dir information in separate lines
const char *settings = "test.cat";

// donot change this information here
char *catalog_path, *dbfile_dir, *tpch_dir = NULL;

extern "C" {
	int yyparse(void);   // defined in y.tab.c
	int yyfuncparse(void);   // defined in yyfunc.tab.c
	void init_lexical_parser (char *); // defined in lex.yy.c (from Lexer.l)
	void close_lexical_parser (); // defined in lex.yy.c
	void init_lexical_parser_func (char *); // defined in lex.yyfunc.c (from Lexerfunc.l)
	void close_lexical_parser_func (); // defined in lex.yyfunc.c
}

extern struct AndList *final;
extern struct FuncOperator *finalfunc;
extern FILE *yyin;

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

	void get_cnf (char *input, CNF &cnf_pred, Record &literal) {
		init_lexical_parser (input);
		if (yyparse() != 0) {
			cout << " Error: can't parse your CNF.\n";
			exit (1);
		}
		cnf_pred.GrowFromParseTree (final, schema (), literal); // constructs CNF predicate
		close_lexical_parser ();
	}

	void get_cnf (char *input, Function &fn_pred) {
		init_lexical_parser_func (input);
		if (yyfuncparse() != 0) {
			cout << " Error: can't parse your CNF.\n";
			exit (1);
		}
		fn_pred.GrowFromParseTree (finalfunc, *(schema ())); // constructs CNF predicate
		close_lexical_parser_func ();
	}

	void get_cnf (CNF &cnf_pred, Record &literal) {
		cout << "\n enter CNF predicate (when done press ctrl-D):\n\t";
		if (yyparse() != 0) {
			cout << " Error: can't parse your CNF.\n";
			exit (1);
		}
		cnf_pred.GrowFromParseTree (final, schema (), literal); // constructs CNF predicate
	}

	void get_file_cnf (const char *fpath, CNF &cnf_pred, Record &literal) {
		yyin = fopen (fpath, "r");
		if (yyin == NULL) {
			cout << " Error: can't open file " << fpath << " for parsing \n";
			exit (1);
		}
		if (yyparse() != 0) {
			cout << " Error: can't parse your CNF.\n";
			exit (1);
		}
		cnf_pred.GrowFromParseTree (final, schema (), literal); // constructs CNF predicate
		// cnf_pred.GrowFromParseTree (final, l_schema (), r_schema (), literal); // constructs CNF predicate over two relations l_schema is the left reln's schema r the right's
		//cnf_pred.Print ();
	}


	void get_sort_order (OrderMaker &sortorder) {
		cout << "\n specify sort ordering (when done press ctrl-D):\n\t ";
		if (yyparse() != 0) {
			cout << " Error: can't parse your CNF \n";
			exit (1);
		}
		Record literal;
		CNF sort_pred;
		sort_pred.GrowFromParseTree (final, schema (), literal); // constructs CNF predicate
		OrderMaker dummy;
		sort_pred.GetSortOrders (sortorder, dummy);
	}
};

void get_cnf (char *input, Schema *left, CNF &cnf_pred, Record &literal) {
	init_lexical_parser (input);
	if (yyparse() != 0) {
		cout << " Error: can't parse your CNF " << input << endl;
		exit (1);
	}
	cnf_pred.GrowFromParseTree (final, left, literal); // constructs CNF predicate
	close_lexical_parser ();
}

void get_cnf (char *input, Schema *left, Schema *right, CNF &cnf_pred, Record &literal) {
	init_lexical_parser (input);
	if (yyparse() != 0) {
		cout << " Error: can't parse your CNF " << input << endl;
		exit (1);
	}
	cnf_pred.GrowFromParseTree (final, left, right, literal); // constructs CNF predicate
	close_lexical_parser ();
}

void get_cnf (char *input, Schema *left, Function &fn_pred) {
	init_lexical_parser_func (input);
	if (yyfuncparse() != 0) {
		cout << " Error: can't parse your arithmetic expr. " << input << endl;
		exit (1);
	}
	fn_pred.GrowFromParseTree (finalfunc, *left); // constructs CNF predicate
	close_lexical_parser_func ();
}

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
	FILE *fp = fopen (settings, "r");
	if (fp) {
		char *mem = (char *) malloc (80 * 3);
		catalog_path = &mem[0];
		dbfile_dir = &mem[80];
		tpch_dir = &mem[160];
		char line[80];
		fgets (line, 80, fp);
		sscanf (line, "%s\n", catalog_path);
		fgets (line, 80, fp);
		sscanf (line, "%s\n", dbfile_dir);
		fgets (line, 80, fp);
		sscanf (line, "%s\n", tpch_dir);
		fclose (fp);
		if (! (catalog_path && dbfile_dir && tpch_dir)) {
			cerr << " Test settings file 'test.cat' not in correct format.\n";
			free (mem);
			exit (1);
		}
	}
	else {
		cerr << " Test settings files 'test.cat' missing \n";
		exit (1);
	}
	cout << " \n** IMPORTANT: MAKE SURE THE INFORMATION BELOW IS CORRECT **\n";
	cout << " catalog location: \t" << catalog_path << endl;
	cout << " tpch files dir: \t" << tpch_dir << endl;
	cout << " heap files dir: \t" << dbfile_dir << endl;
	cout << " \n\n";

	sch=new Schema *[8];
	rel=new relation *[8];

	//curbing memory leaks
	sch[0]=new Schema(catalog_path, supplier);
	sch[1]=new Schema(catalog_path, partsupp);
	sch[2]=new Schema(catalog_path, part);
	sch[3]=new Schema(catalog_path, nation);
	sch[4]=new Schema(catalog_path, customer);
	sch[5]=new Schema(catalog_path, orders);
	sch[6]=new Schema(catalog_path, region);
	sch[7]=new Schema(catalog_path, lineitem);

	rel[0]= new relation (supplier, sch[0], dbfile_dir);
	rel[1]= new relation (partsupp, sch[1], dbfile_dir);
	rel[2]= new relation (part, sch[2], dbfile_dir);
	rel[3]= new relation (nation, sch[3], dbfile_dir);
	rel[4]= new relation (customer, sch[4], dbfile_dir);
	rel[5]= new relation (orders, sch[5], dbfile_dir);
	rel[6]= new relation (region, sch[6], dbfile_dir);
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
