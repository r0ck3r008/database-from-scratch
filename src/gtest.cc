#include "gtest/gtest.h"
#include "lex/comparison.h"
#include "fs/dbfile.h"
#include "lex/comparison_engine.h"
#include "test.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "string.h"
#include "errno.h"

relation *rela;
DBFile dbfile;
const char *dbfile_dir = "out/"; // dir where binary heap files should be stored
const char *tpch_dir ="tpch-dbgen/"; // dir where dbgen tpch files (extension *.tbl) can be found
const char *catalog_path = "db/catalog"; // full path of the catalog file

TEST (DBFILETEST, DBCreate) {

	EXPECT_EQ(dbfile.Create (rela->path(), heap, NULL), 1);
	char tbl_path[100]; // construct path of the tpch flat text file
	sprintf (tbl_path, "%s%s.tbl", tpch_dir, rela->name());
	cout << " tpch file will be loaded from " << tbl_path << endl;

	dbfile.Load ((rela->schema ()), tbl_path);
	dbfile.Close();
}

TEST (DBFILETEST, DBOpen) {

	EXPECT_EQ(dbfile.Open(rela->path()), 1);
	// dbfile.Close();
}

TEST (DBFILETEST, DBClose) {
	EXPECT_EQ(dbfile.Close(), 1);
}

int main (int argc, char **argv) {

	testing::InitGoogleTest(&argc, argv);

	setup(catalog_path, dbfile_dir, tpch_dir);
	/*
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
	}*/

	int stat;
	for(int i=0; i<8; i++) {
	rela = rel [i];
	stat=RUN_ALL_TESTS();
	if(stat)
		break;
	}

	cleanup();
	return stat;
}
