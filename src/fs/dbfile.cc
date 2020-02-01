#include<iostream>
#include<unistd.h>
#include<string.h>

#include "dbfile.h"

DBFile :: DBFile()
{
	this->file=new File;
	this->head=NULL;
	this->dirty=0;
	this->curr_pg=0;
	this->pg=new Page;
}

DBFile :: ~DBFile()
{
	delete this->pg;
	delete this->file;
}

//private functions
void DBFile :: set_dirty()
{
	this->dirty=1;
}

void DBFile :: unset_dirty()
{
	this->dirty=0;
}

int DBFile :: chk_dirty()
{
	return this->dirty ? 1 : 0;
}

void DBFile :: writeback()
{
	this->file->AddPage(this->pg, this->curr_pg++);
	this->pg->EmptyItOut();
	this->unset_dirty();
}

void DBFile :: fetch(off_t pg_num)
{
	if(this->chk_dirty())
		this->writeback();

	this->file->GetPage(this->pg, pg_num);
}


//public functions
int DBFile :: Create(const char *fname, fType type, void *startup)
{
	this->file->Open(0, fname);
	return 1;
}

int DBFile :: Open(const char *fname)
{
	//TODO
	//dummy proofing, use stat to acertain that the file exists
	this->file->Open(1, fname);
	this->fetch(0);
	return 1;
}

void DBFile :: MoveFirst()
{

}

int DBFile :: GetNext(Record **placeholder)
{
	return 1;
}

void DBFile :: Add(Record *placeholder)
{
	off_t curr_len=this->file->GetLength();

	if(curr_len==0 || this->curr_pg==curr_len-1) {
		//this is on latest page
		//check size and writeback if necessary
		if(this->pg->get_curr_size() + placeholder->get_size() >
			PAGE_SIZE)
			this->writeback();
	} else {
		//fetch latest page for reading... this is a read write
		//alternating situation
		this->fetch(curr_len-1);
	}

	if(!this->pg->Append(placeholder)) {
		std :: cerr << "Error in adding record!\n";
		_exit(-1);
	}

	if(!chk_dirty())
		this->set_dirty();
}

void DBFile :: Load(Schema *sch, const char *fname)
{
	FILE *f=NULL;
	if((f=fopen(fname, "r"))==NULL) {
		std :: cerr << "Error in opening the file at " << fname
			<< std :: endl;
		_exit(-1);
	}

	Record *tmp=new Record;
	while(1) {
		int stat=tmp->SuckNextRecord(sch, f);
		if(!stat) {
			if(feof)
				std :: cout << "EOF!\n";
			else
				std :: cout << "Error!\n";
			break;
		}

		this->head=tmp;
		this->Add(tmp);

		tmp=new Record;
	}

	fclose(f);
}

int DBFile :: Close()
{
	if(this->chk_dirty())
		this->writeback();

	this->file->Close();
	return 1;
}
