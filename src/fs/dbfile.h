#ifndef DBFILE_H
#define DBFILE_H

#include"file.h"
#include"record.h"
#include"heap.h"
#include"sorted.h"
#include"lex/comparison_engine.h"
#include"db/schema.h"
#include"glbl/defs.h"

class SortedFile;

struct SortInfo
{
	OrderMaker *order;
	int run_len;
};

class DBFile
{
private:
	HeapFile *heap;
	SortedFile *sorted;
	fType type;

private:
	int get_type(const char *);
public:
	DBFile();
	~DBFile();
	int Create(const char *, fType, SortInfo *);
	int Open(const char *);
	void Add(Record *);
	void Load(Schema *, const char *);
	void MoveFirst();
	int GetNext(Record *);
	int GetNext(Record *, CNF *, Record *);
	int Close();
};

#endif
