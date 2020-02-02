#include <iostream>

#include "test.h"

// make sure that the file path/dir information below is correct
const char *dbfile_dir = "out/"; // dir where binary heap files should be stored
const char *tpch_dir ="tpch-dbgen/"; // dir where dbgen tpch files (extension *.tbl) can be found
const char *catalog_path = "db/catalog"; // full path of the catalog file

using namespace std;

relation *rela;

// load from a tpch file
void test1 () {

	DBFile dbfile;
	cout << " DBFile will be created at " << rela->path () << endl;
	dbfile.Create (rela->path(), heap, NULL);

	char tbl_path[100]; // construct path of the tpch flat text file
	sprintf (tbl_path, "%s%s.tbl", tpch_dir, rela->name());
	cout << " tpch file will be loaded from " << tbl_path << endl;

	dbfile.Load (rela->schema (), tbl_path);
	dbfile.Close ();
}

// sequential scan of a DBfile
void test2 () {

	DBFile dbfile;
	dbfile.Open (rela->path());
	dbfile.MoveFirst ();

	Record *temp=new Record;

	int counter = 0;
	while (dbfile.GetNext (temp) == 1) {
		counter += 1;
		temp->Print (rela->schema());
		if (counter % 10000 == 0) {
			cout << counter << "\n";
		}
		delete temp;
		temp=new Record;
	}
	delete temp;
	cout << " scanned " << counter << " recs \n";
	dbfile.Close ();
}

// scan of a DBfile and apply a filter predicate
void test3 () {

	cout << " Filter with CNF for : " << rela->name() << "\n";

	CNF cnf;
	Record literal;
	rela->get_cnf (cnf, literal);

	DBFile dbfile;
	dbfile.Open (rela->path());
	dbfile.MoveFirst ();

	Record *temp=new Record;

	int counter = 0;
	while (dbfile.GetNext (temp, &cnf, &literal) == 1) {
		counter += 1;
		temp->Print (rela->schema());
		if (counter % 10000 == 0) {
			cout << counter << "\n";
		}
		delete temp;
		temp=new Record;
	}
	delete temp;
	cout << " selected " << counter << " recs \n";
	dbfile.Close ();
}

int main () {

	setup (catalog_path, dbfile_dir, tpch_dir);

	void (*test) ();
	void (*test_ptr[]) () = {&test1, &test2, &test3};

	int tindx = 0;
	while (tindx < 1 || tindx > 3) {
		cout << " select test: \n";
		cout << " \t 1. load file \n";
		cout << " \t 2. scan \n";
		cout << " \t 3. scan & filter \n \t ";
		cin >> tindx;
	}

	int findx = 0;
	while (findx < 1 || findx > 8) {
		cout << "\n select table: \n";
		cout << "\t 1. supplier \n";
		cout << "\t 2. partsupp \n";
		cout << "\t 3. part \n";
		cout << "\t 4. nation \n";
		cout << "\t 5. lineitem \n";
		cout << "\t 6. region \n";
		cout << "\t 7. orders \n";
		cout << "\t 8. customer \n \t ";
		cin >> findx;
	}

	rela = rel[findx-1];
	test = test_ptr [tindx - 1];

	test ();

	cleanup ();
}
