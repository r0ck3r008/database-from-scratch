#include<iostream>
#include<unistd.h>
#include<string.h>

#include "glbl/defs.h"
#include "heap.h"

HeapFile :: HeapFile(File *file, Page *pg)
{
	this->file=file;
	this->head=NULL;
	this->dirty=0;
	this->curr_pg=0;
	this->pg=pg;
	this->cmp=new ComparisonEngine;
}

HeapFile :: ~HeapFile()
{
	delete this->cmp;
}

//private functions
void HeapFile :: set_dirty()
{
	this->dirty=1;
}

void HeapFile :: unset_dirty()
{
	this->dirty=0;
}

int HeapFile :: chk_dirty()
{
	return this->dirty ? 1 : 0;
}

void HeapFile :: writeback()
{
	this->file->AddPage(this->pg, this->curr_pg++);
	this->pg->EmptyItOut();
	this->unset_dirty();
}

void HeapFile :: fetch(off_t pg_num)
{
	if(this->chk_dirty())
		this->writeback();
	else
		this->curr_pg=pg_num;

	this->file->GetPage(this->pg, pg_num);
}


//public functions
int HeapFile :: Create(const char *fname)
{
	if(!this->file->Open(0, fname))
		return 0;
	return 1;
}

int HeapFile :: Open(const char *fname)
{
	//TODO
	//dummy proofing, use stat to acertain that the file exists
	if(!this->file->Open(1, fname))
		return 0;
	this->fetch(0);
	return 1;
}

void HeapFile :: MoveFirst()
{
	if(this->curr_pg)
		this->fetch(0);
	else if(this->chk_dirty())
		this->writeback();
}

int HeapFile :: GetNext(Record *placeholder)
{
	//GetNext doesnt need to care about which page is currently loaded or if
	//the dirty bit is set, since that is taken care of in MoveToFirst

	while(1) {
		int stat=this->pg->GetFirst(placeholder);
		if(!stat) {
			if(this->curr_pg!=this->file->GetLength()-2)
				this->fetch((this->curr_pg+1));
			else
				return 0;
		} else {
			break;
		}
	}

	this->head=placeholder;

	return 1;
}

int HeapFile :: GetNext(Record *placeholder, CNF *cnf, Record *literal)
{
	while(1) {
		int stat=this->GetNext(placeholder);
		if(!stat)
			return 0;

		stat=cmp->Compare(placeholder, literal, cnf);
		if(stat)
			break;
	}
	return 1;
}

void HeapFile :: Add(Record *placeholder)
{
	off_t curr_len=this->file->GetLength();

	if(curr_len==0 || this->curr_pg==curr_len-1) {
		//this is on latest page
		//check size and writeback if necessary
		if(this->pg->get_curr_size() + placeholder->get_size() >
			PAGE_SIZE)
			this->writeback();
	} else {
		//fetch latest page for writing... this is a read write
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

void HeapFile :: Load(Schema *sch, const char *fname)
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
		delete tmp;
		tmp=new Record;
	}

	delete tmp;
	fclose(f);
}

int HeapFile :: Close()
{
	if(this->chk_dirty())
		this->writeback();

	if(!this->file->Close())
		return 0;
	return 1;
}
