#ifndef SORTED_H
#define SORTED_H

#include"mem/bigq.h"
#include"mem/pipe.h"
#include"lex/comparison_engine.h"
#include"lex/comparison.h"
#include"db/schema.h"
#include"file.h"
#include"record.h"
#include"dbfile.h"

class SortedFile
{
	File *file;
	Page *pg;
	BigQ *bigq;
	Pipe *in_pipe, *out_pipe;
	struct SortInfo *s_info;
	int dirty, curr_pg;
	Record *head;
	const char *fname;

private:
	DBFile *setup_int_dbf(int);
	int feed();
	int writeback();
	void set_dirty();
	int unset_dirty();
	int chk_dirty();
	void fetch(int);
public:
	SortedFile(struct SortInfo *);
	~SortedFile();
	int Create(const char *);
	int Open(const char *);
	void Add(Record *);
	void Load(Schema *, const char *);
	void MoveFirst();
	int GetNext(Record *);
	int GetNext(Record *, CNF *, Record *);
	int Close();
};

#endif
