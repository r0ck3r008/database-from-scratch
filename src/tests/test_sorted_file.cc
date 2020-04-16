#define NEED_STRUCT

#include <pthread.h>
#include<math.h>

#include "mem/bigq.h"
#include"glbl/defs.h"
#include"cheader.h"

void test1 ();
void test2 ();
void test3 ();

relation *rela;

int add_data (FILE *src, int numrecs, int &res) {
	DBFile dbfile;
	dbfile.Open (rela->path ());
	Record *temp=new Record;

	int proc = 0;
	int xx = 20000;
	while (proc < numrecs && (res = temp->SuckNextRecord (rela->schema (), src))) {
		dbfile.Add (temp);
		if (proc == xx) cerr << "\t ";
		if (proc % xx == 0) cerr << ".";
		delete temp;
		temp=new Record;
		proc++;
	}

	delete temp;
	dbfile.Close ();
	return (proc);
}


// create a dbfile interactively
void test1 () {
	int runlen = 0;
	while (runlen < 1) {
		cout << "\t\n specify runlength:\n\t ";
		cin >> runlen;
	}

	OrderMaker o;
	rela->get_sort_order (o);
	struct SortInfo startup = {o, runlen};
	DBFile dbfile;
	cout << "\n output to dbfile : " << rela->path () << endl;
	dbfile.Create (rela->path(), Sorted, &startup);
	dbfile.Close ();

	char tbl_path[100];
	sprintf (tbl_path, "%s%s.tbl", tpch_dir, rela->name());
	cout << " input from file : " << tbl_path << endl;

	FILE *tblfile = fopen (tbl_path, "r");

	srand48 (time (NULL));

	int proc = 1, res = 1, tot = 0;
	while (proc && res) {
		int x = 2;
		while (x < 1 || x > 3) {
			cout << "\n select option for : " << rela->path () << endl;
			cout << " \t 1. add a few (1 to 1k recs)\n";
			cout << " \t 2. add a lot (1k to 1e+06 recs) \n";
			cout << " \t 3. run some query \n \t ";

			cin >> x;
		}
		if (x < 3) {
			int rand_num=lrand48()%(int)pow(1e3,x)+(x-1)*1000;
			if(rand_num==0)
				continue;
			proc = add_data (tblfile, rand_num, res);
			tot += proc;
			if (proc)
				cout << "\n\t added " << proc << " recs..so far " << tot << endl;
		}
		else {
			test3 ();
		}
	}
	cout << "\n create finished.. " << tot << " recs inserted\n";
	fclose (tblfile);
}

// sequential scan of a DBfile
void test2 () {

	cout << " scan : " << rela->path() << "\n";
	DBFile dbfile;
	dbfile.Open (rela->path());
	dbfile.MoveFirst ();

	Record *temp=new Record;

	int cnt = 0;
	cerr << "\t";
	while (dbfile.GetNext (temp) && ++cnt) {
		temp->Print (rela->schema());
		if (cnt % 10000) {
			cerr << ".";
		}
		delete temp;
		temp=new Record;
	}

	delete temp;
	cout << "\n scanned " << cnt << " recs \n";
	dbfile.Close ();
}

void test3 () {

	CNF cnf;
	Record literal;
	rela->get_cnf (cnf, literal);

	DBFile dbfile;
	dbfile.Open (rela->path());
	dbfile.MoveFirst ();

	Record *temp=new Record;

	int cnt = 0;
	cerr << "\t";
	while (dbfile.GetNext (temp, &cnf, &literal) && ++cnt) {
		temp->Print (rela->schema());
		if (cnt % 10000 == 0) {
			cerr << ".";
		}
		delete temp;
		temp=new Record;
	}
	cout << "\n query over " << rela->path () << " returned " << cnt << " recs\n";
	dbfile.Close ();

}

int main (int argc, char *argv[]) {

	setup ();

	void (*test_ptr[]) () = {&test1, &test2, &test3};
	void (*test) ();

	int tindx = 0;
	while (tindx < 1 || tindx > 3) {
		cout << " select test option: \n";
		cout << " \t 1. create sorted dbfile\n";
		cout << " \t 2. scan a dbfile\n";
		cout << " \t 3. run some query \n \t ";
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
	rela = rel[findx - 1];

	test = test_ptr [tindx-1];
	test ();

	cleanup ();
	cout << "\n\n";
}
