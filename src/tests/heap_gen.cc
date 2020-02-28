#include <iostream>
#include<unistd.h>

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

void display()
{
	DBFile *dbf=new DBFile;
	if(!dbf->Open(rela->path())){
		cerr << "Cannot Open!\n";
		_exit(-1);
	}

	dbf->MoveFirst();
	Record *tmp=new Record;
	int count=0;
	while(1) {
		if(!dbf->GetNext(tmp))
			break;
		tmp->Print(rela->schema());
		delete tmp;
		tmp=new Record;
		count++;
	}

	cout << "Count= " << count << endl;
	delete tmp;
}

int main () {

	setup ();
	void (*test_ptr[])()={test, display};
	void (*test)();

	int tindx=0;
	while(tindx<1 || tindx>2) {
		cout << "\n Select test: \n";
		cout << "\t 1. Make Heap File \n";
		cout << "\t 2. Display HEap File \n\t";
		cin >> tindx;
	}

	int findx = 0;
	while (findx < 1 || findx > 8) {
		cout << "\n select table: \n";
		cout << "\t 1. nation \n";
		cout << "\t 2. region \n";
		cout << "\t 3. customer \n";
		cout << "\t 4. part \n";
		cout << "\t 5. partsupp \n";
		cout << "\t 6. supplier \n";
		cout << "\t 7. orders \n";
		cout << "\t 8. lineitem \n \t ";
		cin >> findx;
	}

	rela = rel[findx-1];
	test=test_ptr[tindx-1];
	test();
	cleanup ();
}
