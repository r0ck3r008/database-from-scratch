#include"sorted.h"

SortedFile :: SortedFile(File *file, Page *pg, struct SortInfo *s_info)
{
	this->file=file;
	this->pg=pg;
	this->s_info=s_info;
	this->bigq=NULL;
	this->in_pipe=NULL;
	this->out_pipe=NULL;
	this->head=NULL;
	this->dirty=0;
	this->curr_pg=0;
}

void SortedFile :: set_dirty()
{
	this->dirty=1;
}

void SortedFile :: unset_dirty()
{
	this->dirty=0;
}

int SortedFile :: chk_dirty()
{
	return (this->dirty) ? 1 : 0;
}

SortedFile :: ~SortedFile()
{
	if(this->in_pipe!=NULL)
		delete this->in_pipe;
	if(this->out_pipe!=NULL)
		delete this->out_pipe;
	if(this->bigq!=NULL)
		delete this->bigq;
}

int SortedFile :: Create(const char *fname)
{
	int ret=1;

	return ret;
}

int SortedFile :: Open(const char *fname)
{
	int ret=1;

	return ret;
}

void SortedFile :: Add(Record *in)
{

}

void SortedFile :: Load(Schema *sch, const char *fname)
{

}

void SortedFile :: MoveFirst()
{

}

int SortedFile :: GetNext(Record *placeholder)
{
	int ret=1;

	return ret;
}

int SortedFile :: Close()
{
	int ret=1;

	return ret;
}
