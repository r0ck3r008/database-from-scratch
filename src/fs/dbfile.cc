#include "dbfile.h"

DBFile::DBFile()
{
	this->file=NULL;
}

DBFile::~DBFile()
{
}

int DBFile::Create(const char *fname, fType file_type, void *startup)
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

int DBFile::Open(const char *fname)
{
	this->file->Open(1, fname);
	return 0;
}

int DBFile::Close()
{
	return this->file->Close();
}
