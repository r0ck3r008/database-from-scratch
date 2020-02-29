#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>

#include"sorted.h"

SortedHelper :: SortedHelper(struct file_info *f_info)
{
	this->in_pipe=NULL;
	this->out_pipe=NULL;
	this->bigq=NULL;
	this->f_info=f_info;
	this->dirty=0;
	this->curr_pg=0;
}

SortedHelper :: ~SortedHelper()
{
	if(this->in_pipe!=NULL)
		delete in_pipe;
	if(this->out_pipe!=NULL)
		delete out_pipe;
	if(this->bigq!=NULL)
		delete bigq;
}

void SortedHelper :: set_create(int val)
{
	this->create=val;
}

int SortedHelper :: get_create()
{
	return this->create;
}

void SortedHelper :: set_dirty()
{
	if(this->in_pipe!=NULL)
		delete in_pipe;
	if(this->out_pipe!=NULL)
		delete out_pipe;
	if(this->bigq!=NULL)
		delete bigq;

	this->in_pipe=new Pipe(100);
	this->out_pipe=new Pipe(100);
	this->bigq=new BigQ(in_pipe, out_pipe, this->f_info->s_info->order,
				this->f_info->s_info->run_len);

	this->dirty=1;
}

int SortedHelper :: feed()
{
	if(!this->create) {
		DBFile *dbf=NULL;
		if((dbf=this->setup_dbf(0))==NULL)
			return 0;

		Record *tmp=new Record;

		while(1) {
			if(!dbf->GetNext(tmp))
				break;

			this->in_pipe->Insert(tmp);
			delete tmp;
			tmp=new Record;
		}

		delete tmp;
		delete dbf;
	}

	this->in_pipe->ShutDown();
	return 1;
}

DBFile *SortedHelper :: setup_dbf(int create_flag)
{
	DBFile *int_dbf=new DBFile;
	if(create_flag)
		if(!int_dbf->Create(tmp_name, Sorted, this->f_info->s_info))
			return NULL;
	else
		if(!int_dbf->Open(this->f_info->fname))
			return NULL;

	return int_dbf;
}

int SortedHelper :: writeback(DBFile *dbf)
{
	Record *tmp=new Record;
	while(1) {
		int stat=this->out_pipe->Remove(tmp);
		if(!stat)
			break;

		if(dbf==NULL)
			this->Add(tmp);
		else
			dbf->Add(tmp);
		delete tmp;
		tmp=new Record;
	}

	delete tmp;
	return 1;
}

int SortedHelper :: reboot()
{
	if(!this->f_info->file->Close())
		return 0;
	delete this->f_info->file;
	this->f_info->pg->EmptyItOut();
	delete this->f_info->pg;

	if(remove(this->f_info->fname)<0) {
		std :: cerr << "Error in removing Old file!\n"
			<< strerror(errno) << std :: endl;
		_exit(-1);
	}

	if(rename(tmp_name, this->f_info->fname)<0) {
		std :: cerr << "Error in renaming this file!"
			<< strerror(errno) << std :: endl;
		_exit(-1);
	}

	this->f_info->file=new File;
	this->f_info->pg=new Page;
	if(!this->f_info->file->Open(1, this->f_info->fname))
		return 0;

	return 1;
}

void SortedHelper :: Add(Record *placeholder)
{
	if(!this->chk_dirty())
		this->set_dirty();

	this->in_pipe->Insert(placeholder);
}

int SortedHelper :: unset_dirty()
{
	if(!this->feed())
		return 0;

	DBFile *int_dbf=NULL;
	if(!this->create) {
		int_dbf=this->setup_dbf(1);
	}
	if(!this->writeback(int_dbf))
		return 0;

	if(!this->create)
		if(!this->reboot())
			return 0;

	this->dirty=0;
}

int SortedHelper :: fetch_curr_pg()
{
	return this->curr_pg;
}

void SortedHelper :: set_curr_pg(int pg_num)
{
	this->curr_pg=pg_num;
}

int SortedHelper :: chk_dirty()
{
	return ((this->dirty) ? 1 : 0);
}
