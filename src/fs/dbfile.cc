#include<iostream>
#include<unistd.h>
#include<string.h>

#include "dbfile.h"

DBFile :: DBFile()
{
	this->head=new Record;
	this->file=NULL;
	this->pg=new Page;
	page_num=0;
}

DBFile :: ~DBFile()
{
	head=NULL;
	delete pg;
	delete file;
}

int DBFile :: Create(const char *fname, fType file_type, void *startup)
{
	if(this->file==NULL){
		file=new File;
		this->file->Open(0, fname);
	}else{
		if(!this->Open(fname))
			return 0;
	}

	return 1;
}

int DBFile :: Open(const char *fname)
{
	this->file->Open(1, fname);
	return 0;
}

void DBFile :: MoveFirst()
{
	head=start;
}

//TODO
//check if the new page pointer used to add current page to the file is now reset
void DBFile :: Add(Record *new_rec)
{
	this->head=new_rec;
	if(this->pg->get_curr_size() + new_rec->get_size() >= PAGE_SIZE)
		this->file->AddPage(this->pg, this->page_num);

	//try to append now
	if(!this->pg->Append(new_rec)) {
		//Error
		std :: cerr << "Error in adding the new record!"
			<< std ::endl;
		_exit(-1);
	}
	//housekeeping
	this->head=new_rec;
	this->page_num++;
}

int DBFile :: GetNext(Record *placeholder)
{
	placeholder=head;
	head=this->pg->get_next();
	return 1;
}

void DBFile :: Load(Schema *schma, char *fname)
{
	FILE *f=NULL;
	if((f=fopen(fname, "r"))==NULL){
		std :: cerr << "Error in opening the file " << fname
			<< std :: endl;
		_exit(-1);
	}

	int stat=1;
	Record *tmp=NULL;
	for(int i=0; !feof(f); i++){
		tmp=new Record;
		if(!tmp->SuckNextRecord(schma, f) && !feof(f)) {
			std :: cerr << "Error in reading from " << fname
				<< std :: endl;
			break;
		}
		if(!i)
			this->start=tmp;
		this->Add(tmp);
	}

	fclose(f);
}

int DBFile :: Close()
{
	return this->file->Close();
}
