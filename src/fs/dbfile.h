#ifndef DBFILE_H
#define DBFILE_H

#include"comparison_engine.h"
#include"record.h"
#include"db/schema.h"
#include"heap.h"

typedef enum
{
	Heap, Sorted, tree;
} fType;

struct SortInfo
{
	OrderMaker *order;
	int run_len;
};

class DBFile
{
private:
	Heap *heap;
	Sorted *sorted;

public:
	DBFile();
	~DBFile();
	int Open(const char *);
	int Create(const char *, fType, SortInfo *);
	void Add(Record *);
	void Load(Schema *, const char *);
	void MoveFirst();
	int GetNext(Record *);
	int GetNext(Record *, CNF *, Record *);
	int Close();
};

#endif
