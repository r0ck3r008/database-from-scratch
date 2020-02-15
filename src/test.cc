#include "test.h"
#include "mem/bigq.h"
#include <pthread.h>

relation *rela;

void *producer (void *arg) {

	Pipe *myPipe = (Pipe *) arg;

	Record *temp=new Record;
	int counter = 0;

	DBFile dbfile;
	dbfile.Open (rela->path ());
	cout << " producer: opened DBFile " << rela->path () << endl;
	dbfile.MoveFirst ();

	while (dbfile.GetNext (temp) == 1) {
		counter += 1;
		if (counter%100000 == 0) {
			cerr << " producer: " << counter << endl;
		}
		myPipe->Insert (temp);
		delete temp;
		temp=new Record;
	}

	delete temp;

	dbfile.Close ();
	myPipe->ShutDown ();

	cout << " producer: inserted " << counter << " recs into the pipe\n";

	return NULL;
}

void *consumer (void *arg) {

	testutil *t = (testutil *) arg;

	ComparisonEngine ceng;

	DBFile dbfile;
	char outfile[100];

	if (t->write) {
		sprintf (outfile, "%s.bigq", rela->path ());
		dbfile.Create (outfile, heap, NULL);
	}

	int err = 0;
	int i = 0;

	Record rec[2];
	Record *last = NULL, *prev = NULL;

	while (t->pipe->Remove (&rec[i%2])) {
		prev = last;
		last = &rec[i%2];

		if (prev && last) {
			if (ceng.Compare (prev, last, t->order) == 1) {
				err++;
			}
			if (t->write) {
				dbfile.Add (prev);
			}
		}
		if (t->print) {
			last->Print (rela->schema ());
		}
		i++;
	}

	cout << " consumer: removed " << i << " recs from the pipe\n";

	if (t->write) {
		if (last) {
			dbfile.Add (last);
		}
		cerr << " consumer: recs removed written out as heap DBFile at " << outfile << endl;
		dbfile.Close ();
	}
	cerr << " consumer: " << (i - err) << " recs out of " << i << " recs in sorted order \n";
	if (err) {
		cerr << " consumer: " <<  err << " recs failed sorted order test \n" << endl;
	}

	return NULL;
}


void test1 (int option, int runlen) {

	// sort order for records
	OrderMaker sortorder;
	rela->get_sort_order (sortorder);

	int buffsz = 100; // pipe cache size
	Pipe input (buffsz);
	Pipe output (buffsz);

	// thread to dump data into the input pipe (for BigQ's consumption)
	pthread_t thread1;
	pthread_create (&thread1, NULL, producer, (void *)&input);

	// thread to read sorted data from output pipe (dumped by BigQ)
	pthread_t thread2;
	testutil tutil = {&output, &sortorder, false, false};
	if (option == 2) {
		tutil.print = true;
	}
	else if (option == 3) {
		tutil.write = true;
	}
	pthread_create (&thread2, NULL, consumer, (void *)&tutil);

	BigQ bq (&input, &output, &sortorder, runlen);

	pthread_join (thread1, NULL);
	pthread_join (thread2, NULL);
}

// load from a tpch file
void test_dbfile () {

	DBFile dbfile;
	cout << " DBFile will be created at " << rela->path () << endl;
	dbfile.Create (rela->path(), heap, NULL);

	char tbl_path[100]; // construct path of the tpch flat text file
	sprintf (tbl_path, "%s%s.tbl", tpch_dir, rela->name());
	cout << " tpch file will be loaded from " << tbl_path << endl;

	dbfile.Load (rela->schema (), tbl_path);
	dbfile.Close ();
}


int main (int argc, char *argv[]) {

	setup ();
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
	test_dbfile();

	tindx = 0;
	while (tindx < 1 || tindx > 3) {
		cout << " select test option: \n";
		cout << " \t 1. sort \n";
		cout << " \t 2. sort + display \n";
		cout << " \t 3. sort + write \n\t ";
		cin >> tindx;
	}

	findx = 0;
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
	rela = rel[findx - 1];

	int runlen;
	cout << "\t\n specify runlength:\n\t ";
	cin >> runlen;

	test1 (tindx, runlen);

	cleanup ();
}
