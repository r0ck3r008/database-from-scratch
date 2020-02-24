#include<unistd.h>

#include"glbl/defs.h"
#include"sorted.h"

void SortedFile :: set_dirty()
{
	if(this->in_pipe==NULL)
		this->in_pipe=new Pipe(100);
	if(this->out_pipe==NULL)
		this->out_pipe=new Pipe(100);
	if(this->bigq==NULL)
		this->bigq=new BigQ(this->in_pipe, this->out_pipe,
					this->s_info->order,
					this->s_info->run_len);
	this->dirty=1;
}

void SortedFile :: unset_dirty()
{
	this->in_pipe->ShutDown();
	//TODO merging
	this->dirty=0;
}

int SortedFile :: chk_dirty()
{
	return (this->dirty) ? 1 : 0;
}

void SortedFile :: fetch(int pg_num)
{
	if(this->chk_dirty())
		this->unset_dirty();

	this->pg->EmptyItOut();
	this->file->GetPage(this->pg, pg_num);
	this->curr_pg=pg_num;
}

//public
SortedFile :: SortedFile(struct SortInfo *s_info)
{
	this->file=new File;
	this->pg=new Page;
	this->s_info=s_info;
	this->bigq=NULL;
	this->in_pipe=NULL;
	this->out_pipe=NULL;
	this->head=NULL;
	this->dirty=0;
	this->curr_pg=0;
}

SortedFile :: ~SortedFile()
{
	if(this->in_pipe!=NULL)
		delete this->in_pipe;
	if(this->out_pipe!=NULL)
		delete this->out_pipe;
	if(this->bigq!=NULL)
		delete this->bigq;
	delete this->pg;
	delete this->file;
}

int SortedFile :: Create(const char *fname)
{
	int ret=1;
	if(!this->file->Open(0, fname))
		ret=0;
	this->file->set_type(Sorted);
	return ret;
}

int SortedFile :: Open(const char *fname)
{
	int ret=1;
	if(!this->file->Open(1, fname))
		ret=0;
	this->fetch(0);
	return ret;
}

void SortedFile :: Add(Record *in)
{
	if(!this->chk_dirty())
		this->set_dirty();

	this->in_pipe->Insert(in);
}

void SortedFile :: Load(Schema *sch, const char *fname)
{
	FILE *f=NULL;
	if((f=fopen(fname, "r"))==NULL) {
		std :: cerr << "Error in opening the file "
			<< fname << "!\n";
		_exit(-1);
	}

	Record *tmp=new Record;
	while(1) {
		int stat=tmp->SuckNextRecord(sch, f);
		if(!stat) {
			if(!feof)
				std :: cerr << "Error!\n";
			else
				std :: cerr << "EOF!\n";
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

void SortedFile :: MoveFirst()
{
	if(this->chk_dirty())
		this->unset_dirty();

	if(!this->curr_pg)
		this->fetch(0);
}

int SortedFile :: GetNext(Record *placeholder)
{
	int ret=1;
	if(this->chk_dirty())
		this->MoveFirst();

	int stat=this->pg->GetFirst(placeholder);
	if(!stat) {
		if(this->curr_pg!=(this->file->GetLength())-2) {
			this->fetch((this->curr_pg+1));
			ret=this->GetNext(placeholder);
		} else {
			ret=0;
		}
	} else {
		this->head=placeholder;
	}

	return ret;
}

int SortedFile :: Close()
{
	int ret=1;

	return ret;
}
