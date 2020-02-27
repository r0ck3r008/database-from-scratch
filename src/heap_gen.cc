#include <iostream>

#include"glbl/timer.h"
#include "test.h"

using namespace std;

relation *rela;

// load from a tpch file
void test () {

	DBFile dbfile;
	cout << " DBFile will be created at " << rela->path () << endl;
	dbfile.Create (rela->path(), Heap, NULL);

	char tbl_path[100]; // construct path of the tpch flat text file
	sprintf (tbl_path, "%s%s.tbl", tpch_dir, rela->name());
	cout << " tpch file will be loaded from " << tbl_path << endl;

	dbfile.Load (rela->schema (), tbl_path);
	dbfile.Close ();
}

int main () {

	setup ();

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
	test ();

	cleanup ();
}
