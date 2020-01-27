#ifndef DBFILE_H
#define DBFILE_H

#include"mem/two_way_list.cc"
#include"file.h"
#include"db/schema.h"

typedef enum
{
	heap, sorted, tree
} fType;

class DBFile
{
	Record *head;
	Record *start;
	File *file;
	Page *pg;
	off_t page_num;
public:
	DBFile();
	~DBFile();
	int Create(const char *, fType, void *);
	int Open(const char *);
	void MoveFirst();
	void Add(Record *);
	int GetNext(Record *);
	void Load(Schema *, char *);
	int Close();
};


#endif
