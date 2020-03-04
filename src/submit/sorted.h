#ifndef SORTED_H
#define SORTED_H

#define NEED_STRUCTS

#include"record.h"
#include"file.h"
#include"bigq.h"
#include"pipe.h"
#include"comparison.h"
#include"schema.h"
#include"dbfile.h"
#include"defs.h"

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
	int setup_dbf(DBFile **, int);
	int feed();
	int writeback(int);

public:
	SortedHelper(struct file_info *);
	~SortedHelper();
	void set_create(int);
	int get_create();
	int fetch(int, int);
	void set_dirty(int);
	int unset_dirty(int);
	int fetch_curr_pg();
	void set_curr_pg(int);
	int chk_dirty();
	void Add(Record *, int);
	int reboot();
};

class SortedFile
{
private:
	SortedHelper *helper;
	Record *head;
	OrderMaker *query;
	int pseudo;
	struct file_info *f_info;

public:
	SortedFile(struct SortInfo *,
			const char *, int);
	~SortedFile();
	int Create();
	int Open();
	void MoveFirst();
	void Add(Record *);
	void Load(Schema *, const char *);
	int GetNext(Record *);
	int GetNext(Record *, CNF *, Record *);
	int get_next_no_query(Record *, CNF *, Record *);
	int get_next_query(Record *, CNF *, Record *);
	int binary_search(Record *, CNF *, Record *);
	int Close();
};

#endif
