#include <iostream>

#include"glbl/timer.h"
#include "test.h"

using namespace std;

relation *rela;

// load from a tpch file
void test1 () {

	DBFile dbfile;
	cout << " DBFile will be created at " << rela->path () << endl;
	dbfile.Create (rela->path(), Heap, NULL);

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
	timer t;

	DBFile dbfile;
	dbfile.Open (rela->path());
	dbfile.MoveFirst ();

	Record *temp=new Record;

	t.start_timer();
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
	t.stop_timer();
	struct timeval diff=t.get_tt();
	delete temp;
	cout << " selected " << counter << " recs \n";
	cout << "Time taken: " << diff.tv_usec/1000.00 << " milliseconds!\n";
	dbfile.Close ();
}

int main () {

	setup ();

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
		cout << "\t 5. customer \n";
		cout << "\t 6. orders \n";
		cout << "\t 7. region \n";
		cout << "\t 8. lineitem \n \t ";
		cin >> findx;
	}

	rela = rel[findx-1];
	test = test_ptr [tindx - 1];

	test ();

	cleanup ();
}
