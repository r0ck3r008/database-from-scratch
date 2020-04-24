#ifndef DBFILE_H
#define DBFILE_H
#define NEED_STRUCTS

#include"file.h"
#include"record.h"
#include"heap.h"
#include"sorted.h"
#include"comparison_engine.h"
#include"schema.h"
#include"defs.h"

class SortedFile;

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
