#include<unistd.h>

#include"glbl/defs.h"
#include"sorted.h"

void SortedFile :: set_dirty()
{
	//flush old pipes and bigq and generate new
	if(this->in_pipe!=NULL)
		delete this->in_pipe;
	this->in_pipe=new Pipe(100);

	if(this->out_pipe!=NULL)
		delete this->out_pipe;
	this->out_pipe=new Pipe(100);

	if(this->bigq!=NULL)
		delete this->bigq;
	this->bigq=new BigQ(this->in_pipe, this->out_pipe,
				this->s_info->order,
				this->s_info->run_len);
	this->dirty=1;
}

DBFile *SortedFile :: setup_int_dbf(int create)
{
	DBFile *int_dbf=new DBFile;
	if(create) {
		if(!int_dbf->Create("bin/tmp_sorted.bin", Sorted,
					this->s_info))
			return NULL;
	} else {
		if(!int_dbf->Open(this->fname))
			return NULL;
	}

	return int_dbf;
}

int SortedFile :: feed()
{
	DBFile *int_dbf=NULL;
	if((int_dbf=this->setup_int_dbf(0)))
		return 0;

	Record *tmp=new Record;
	while(1) {
		if(!int_dbf->GetNext(tmp))
			break;
		this->in_pipe->Insert(tmp);
		delete tmp;
		tmp=new Record;
	}

	this->in_pipe->ShutDown();
	if(!int_dbf->Close())
		return 0;

	delete int_dbf;
	delete tmp;

	return 1;
}

int SortedFile :: writeback()
{
	DBFile *int_dbf=NULL;
	if((int_dbf=this->setup_int_dbf(1))==NULL)
		return 0;

	Record *tmp=new Record;
	while(1) {
		if(!this->out_pipe->Remove(tmp))
			break;
		int_dbf->Add(tmp);
		delete tmp;
		tmp=new Record;
	}

	if(!int_dbf->Close())
		return 0;

	delete tmp;
	delete int_dbf;

	return 1;
}

int SortedFile :: unset_dirty()
{
	if(!this->feed())
		return 0;

	if(!this->writeback())
		return 0;

	this->dirty=0;
	return 1;
}

int SortedFile :: chk_dirty()
{
	return (this->dirty) ? 1 : 0;
}

void SortedFile :: fetch(int pg_num)
{
	if(this->chk_dirty())
		if(!this->unset_dirty()) {
			std :: cerr << "Error in writingback!\n";
			_exit(-1);
		}

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
	this->fname=fname;
	return ret;
}

int SortedFile :: Open(const char *fname)
{
	int ret=1;
	if(!this->file->Open(1, fname))
		ret=0;
	this->fetch(0);
	this->fname=fname;
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
		if(!this->unset_dirty()) {
			std :: cerr << "Error in writingback!\n";
			_exit(-1);
		}

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
	if(this->chk_dirty())
		if(!this->unset_dirty())
			ret=0;

	return ret;
}
