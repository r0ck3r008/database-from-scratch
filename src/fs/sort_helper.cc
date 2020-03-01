#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
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

int SortedHelper :: fetch(int pg_num)
{
	if(this->chk_dirty())
		if(!this->unset_dirty())
			return 0;

	int curr_len=this->f_info->file->GetLength();
	if(pg_num<curr_len) {
		this->f_info->pg->EmptyItOut();
		this->f_info->file->GetPage(this->f_info->pg, pg_num);
		this->set_curr_pg(pg_num);
		return 1;
	} else {
		return 0;
	}
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
	this->bigq=new BigQ(in_pipe, out_pipe, &(this->f_info->s_info->order),
				this->f_info->s_info->run_len);

	this->dirty=1;
}

int SortedHelper :: feed()
{
	if(!this->create) {
		DBFile **dbf=new DBFile *;
		if(!this->setup_dbf(dbf, 0))
			return 0;

		Record *tmp=new Record;
		while(1) {
			if(!(*dbf)->GetNext(tmp))
				break;
			this->in_pipe->Insert(tmp);
			delete tmp;
			tmp=new Record;
			std :: cerr << "Getting next!\n";
		}

		if(!(*dbf)->Close())
			return 0;
		delete tmp;
		delete *dbf;
		delete dbf;
	}

	this->in_pipe->ShutDown();
	return 1;
}

int SortedHelper :: setup_dbf(DBFile **dbf, int create_flag)
{
	*dbf=new DBFile;
	if(create_flag) {
		if(!(*dbf)->Create(tmp_name, Sorted, this->f_info->s_info))
			return 0;
	} else {
		if(!(*dbf)->Open(this->f_info->fname))
			return 0;
	}

	return 1;
}

int SortedHelper :: writeback(DBFile **dbf)
{
	Record *tmp=new Record;
	while(1) {
		int stat=this->out_pipe->Remove(tmp);
		if(!stat)
			break;

		if(*dbf==NULL)
			this->Add(tmp, 1);
		else
			(*dbf)->Add(tmp);
		delete tmp;
		tmp=new Record;
	}

	if(*dbf!=NULL) {
		if(!(*dbf)->Close())
			return 0;
		delete *dbf;
		delete dbf;
	}

	struct stat buf;
	if(stat("bin/runs.bin", &buf)==0) {
		if(unlink("bin/runs.bin")<0) {
			std :: cerr << "Error in deleting old runs file!"
				<< strerror(errno) << std :: endl;
			_exit(-1);
		}
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

	if(unlink(this->f_info->fname)<0) {
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

void SortedHelper :: Add(Record *placeholder, int internal)
{
	if(internal && this->get_create()) {
		off_t curr_len=this->f_info->file->GetLength();
		int curr_pg=this->fetch_curr_pg();
		if(curr_len==0 || curr_pg==curr_len-1) {
			//this is on latest page
			//check size and writeback if necessary
			if(this->f_info->pg->get_curr_size() +
					placeholder->get_size() > PAGE_SIZE) {
				this->f_info->file->AddPage(this->f_info->pg,
								curr_pg);
				this->f_info->pg->EmptyItOut();
				this->curr_pg++;
			}
		} else {
			//fetch latest page for writing... this is a read write
			//alternating situation
			this->fetch(curr_len-1);
		}

		if(!this->f_info->pg->Append(placeholder)) {
			std :: cerr << "Error in adding record!\n";
			_exit(-1);
		}
	} else {
		this->in_pipe->Insert(placeholder);
	}
}

int SortedHelper :: unset_dirty()
{
	this->dirty=0;
	DBFile **int_dbf=new DBFile *;
	*int_dbf=NULL;

	if(!this->feed())
		return 0;

	if(!this->create) {
		if(!this->setup_dbf(int_dbf, 1)) {
			return 0;
		}
	}
	if(!this->writeback(int_dbf))
		return 0;

	if(!this->create)
		if(!this->reboot())
			return 0;
	return 1;
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
