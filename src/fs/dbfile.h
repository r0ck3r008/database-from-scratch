#ifndef DBFILE_H
#define DBFILE_H

#include"file.h"
#include"db/schema.h"
#include"lex/comparison.h"

typedef enum
{
	heap, sorted, tree
} fType;

class DBFile
{
	//vars
	File *file;
	Page *pg;
	off_t curr_pg;
	Record *head;
	int dirty;
private:
	//functions
	void writeback();
	void fetch(off_t);
	void set_dirty();
	void unset_dirty();
	int chk_dirty();
public:
	DBFile();
	~DBFile();
	int Create(const char *, fType, void *);
	int Open(const char *);
	void MoveFirst();
	void Add(Record *);
	int GetNext(Record **);
	int GetNext(Record *, CNF *, Record *);
	void Load(Schema *, const char *);
	int Close();
};


#endif
