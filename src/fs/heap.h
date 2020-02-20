#ifndef HEAP_H
#define HEAP_H

#include"file.h"
#include"db/schema.h"
#include"lex/comparison.h"

class Heap
{
	//vars
	File *file;
	Page *pg;
	off_t curr_pg;
	Record *head;
	int dirty;
	ComparisonEngine *cmp;
private:
	//functions
	void writeback();
	void fetch(off_t);
	void set_dirty();
	void unset_dirty();
	int chk_dirty();
public:
	Heap();
	~Heap();
	int Create(const char *);
	int Open(const char *);
	void MoveFirst();
	void Add(Record *);
	int GetNext(Record *);
	int GetNext(Record *, CNF *, Record *);
	void Load(Schema *, const char *);
	int Close();
};


#endif
