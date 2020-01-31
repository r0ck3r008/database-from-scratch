#include<iostream>
#include<unistd.h>
#include<string.h>

#include "dbfile.h"

DBFile :: DBFile()
{
	this->file=new File;
	this->pg=new Page;
	this->curr_pg=0;
	this->head=NULL;;
}

DBFile :: ~DBFile()
{
	delete this->pg;
	delete this->file;
}

void DBFile :: writeback()
{
	this->file->AddPage(this->pg, curr_pg++);
	this->pg->EmptyItOut();
}

void DBFile :: fetch(off_t pg_num, int wrtbk)
{
	if(wrtbk)
		this->writeback();
	this->file->GetPage(this->pg, pg_num);
	this->curr_pg=pg_num;
}

int DBFile :: Create(const char *fname, fType type, void *startup)
{
	this->file->Open(0, fname);
	return 1;
}

int DBFile :: Open(const char *fname)
{
	this->file->Open(1, fname);
	this->fetch(0, 0);
	return 1;
}

void DBFile :: MoveFirst()
{
	if(this->curr_pg)
		this->fetch(0, 1);

	this->pg->myRecs->MoveToStart();
	this->head=this->pg->myRecs->Current(0);
}

void DBFile :: Add(Record *new_rec)
{
	if(this->pg->get_curr_size() + new_rec->get_size() > PAGE_SIZE)
		//writeback to disk
		this->writeback();

	if(!this->pg->Append(new_rec)) {
		std :: cerr << "Error in adding new rec!" << std :: endl;
		_exit(-1);
	}
}

int DBFile :: GetNext(Record *placeholder)
{
	if(!this->pg->myRecs->RightLength())
		this->fetch(this->curr_pg++, 1);

	this->head=this->pg->myRecs->Current(0);
	placeholder=this->head;
	this->pg->myRecs->Advance();
	return 1;
}

void DBFile :: Load(Schema *sch, char *fname)
{
	FILE *f=NULL;
	if((f=fopen(fname, "r"))==NULL) {
		std :: cerr << "Error in opening file at " << fname
			<< std :: endl;
		_exit(-1);
	}

	//NOTE
	//feof returns 1 if it is the end of file else 0
	Record *tmp=NULL;
	while(1) {
		int stat=0;
		tmp=new Record;
		stat=tmp->SuckNextRecord(sch, f);
		if(!stat) {
			if(feof)
				std :: cout << "EOF!" << std :: endl;
			else
				std :: cout << "Error in getting new record "
					<< std :: endl;
			break;
		}
		this->head=tmp;
		this->Add(tmp);
	}

	std :: cout << "Exiting!\n";
	fclose(f);
}

int DBFile :: Close()
{
	this->writeback();
	if(!this->file->Close())
		return 0;
	return 1;
}
