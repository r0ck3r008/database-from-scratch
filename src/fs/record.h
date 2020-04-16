#ifndef RECORD_H
#define RECORD_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "glbl/defs.h"
#include "db/schema.h"
#include "file.h"
#include "parser/parse_tree.h"
#include "comparator/comparison.h"
#include "comparator/comparison_engine.h"
#include "comparator/function.h"



// Basic record data structure. Data is actually stored in "bits" field. The layout of bits is as follows:
//	1) First sizeof(int) bytes: length of the record in bytes
//	2) Next sizeof(int) bytes: byte offset to the start of the first att
//	3) Byte offset to the start of the att in position numAtts
//	4) Bits encoding the record's data

class Record {

	friend class ComparisonEngine;
	friend class Page;
	friend class Function;

private:
	char *bits;
	char *GetBits ();
	void SetBits (char *bits);
	void CopyBits(char *bits, int b_len);

public:
	Record ();
	~Record();

	// suck the contents of the record fromMe into this; note that after
	// this call, fromMe will no longer have anything inside of it
	void Consume (Record *fromMe);

	int check_null();

	// make a copy of the record fromMe; note that this is far more
	// expensive (requiring a bit-by-bit copy) than Consume, which is
	// only a pointer operation
	void Copy (Record *copyMe);

	int get_size();

	int ComposeRecord(Schema *, const char *);

	// reads the next record from a pointer to a text file; also requires
	// that the schema be given; returns a 0 if there is no data left or
	// if there is an error and returns a 1 otherwise
	int SuckNextRecord (Schema *mySchema, FILE *textFile);

	// this projects away various attributes...
	// the array attsToKeep should be sorted, and lists all of the attributes
	// that should still be in the record after Project is called.  numAttsNow
	// tells how many attributes are currently in the record
	void Project (int *attsToKeep, int numAttsToKeep, int numAttsNow);

	// takes two input records and creates a new record by concatenating them;
	// this is useful for a join operation
	void MergeRecords (Record *left, Record *right, int numAttsLeft,
				int numAttsRight, int *attsToKeep,
				int numAttsToKeep, int startOfRight);

	char *deserialize(Schema *);
	// prints the contents of the record; this requires
	// that the schema also be given so that the record can be interpreted
	void Print (Schema *mySchema);
};

#endif
