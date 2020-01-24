#ifndef DBFILE_H
#define DBFILE_H

#include"mem/two_way_list.cc"
#include"file.h"

enum fType
{
	heap, sorted, tree;
};

class DBFile
{
	Record *pointer;
	TwoWAyList <Page> pgs;
	File *file;
public:
	DBFile();
	~DBFile();
	int Create(char *, fType, void *);
	int Open(char *);
	int Close();
};


#endif
