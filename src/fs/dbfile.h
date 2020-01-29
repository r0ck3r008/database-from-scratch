#ifndef DBFILE_H
#define DBFILE_H

#include"mem/two_way_list.cc"
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
private:
	//functions
	void writeback();
	void fetch(off_t);
public:
	DBFile();
	~DBFile();
	int Create(const char *, fType, void *);
	int Open(char *);
	void MoveFirst();
	void Add(Record *);
	void Load(Schema *, char *);
	int Close();
};


#endif
