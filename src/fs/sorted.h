#ifndef SORTED_H
#define SORTED_H

#define NEED_STRUCTS

#include"record.h"
#include"file.h"
#include"mem/bigq.h"
#include"mem/pipe.h"
#include"lex/comparison.h"
#include"db/schema.h"
#include"dbfile.h"
#include"glbl/defs.h"

static const char *tmp_name="bin/sorted_tmp.bin";

struct file_info
{
	struct SortInfo *s_info;
	const char *fname;
	File *file;
	Page *pg;

public:
	file_info(struct SortInfo *,
			const char *,
			File *, Page *);
};

class DBFile;
class SortedHelper
{
private:
	Pipe *in_pipe;
	Pipe *out_pipe;
	BigQ *bigq;
	struct file_info *f_info;
	int dirty, create, curr_pg;
private:
	DBFile *setup_dbf(int);
	int feed();
	int writeback(DBFile *);
	int reboot();

public:
	SortedHelper(struct file_info *);
	~SortedHelper();
	void set_create(int);
	int get_create();
	int fetch(int);
	void set_dirty();
	int unset_dirty();
	int fetch_curr_pg();
	void set_curr_pg(int);
	int chk_dirty();
	void Add(Record *, int);
};

class SortedFile
{
private:
	SortedHelper *helper;
	Record *head;
	struct file_info *f_info;

public:
	SortedFile(struct SortInfo *,
			const char *);
	~SortedFile();
	int Create();
	int Open();
	void MoveFirst();
	void Add(Record *);
	void Load(Schema *, const char *);
	int GetNext(Record *);
	int GetNext(Record *, CNF *, Record *);
	int Close();
};

#endif
