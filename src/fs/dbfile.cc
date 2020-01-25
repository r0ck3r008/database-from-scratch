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

void DBFile :: Load(Schema *schma, char *fname)
{
	FILE *f=NULL;
	if((f=fopen(fname, "r"))==NULL){
		std :: cerr << "Error in opening the file " << fname << std :: endl;
		_exit(-1);
	}

	int stat=1;
	Record *tmp=NULL;
	for(int i=0; ; i++){
		tmp=new Record;
		if(!tmp->SuckNextRecord(schma, f) && feof(f))
			//Error
			break;
		if(!i)
			this->start=tmp;
		this->head=tmp;

		if(this->pg->get_curr_size() + tmp->get_size() < PAGE_SIZE)
			if(!this->pg->Append(tmp))
				//Error
				break;
		else
			this->file->AddPage(this->pg, this->page_num);
	}

	fclose(f);
}

int DBFile :: Close()
{
	return this->file->Close();
}
