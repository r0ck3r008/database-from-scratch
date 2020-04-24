#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

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

int SortedHelper :: fetch(int pg_num, int pseudo)
{
	if(this->chk_dirty())
		if(!this->unset_dirty(pseudo))
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

void SortedHelper :: set_dirty(int pseudo)
{
	if(!pseudo) {
		if(this->in_pipe!=NULL)
			delete in_pipe;
		if(this->out_pipe!=NULL)
			delete out_pipe;
		if(this->bigq!=NULL)
			delete bigq;

		this->in_pipe=new Pipe(100);
		this->out_pipe=new Pipe(100);
		this->bigq=new BigQ(in_pipe, out_pipe,
					&(this->f_info->s_info->order),
					this->f_info->s_info->run_len);
	}

	this->dirty=1;
}

int SortedHelper :: feed()
{
	if(!this->create) {
		DBFile *dbf=new DBFile;
		if(!dbf->Open(this->f_info->fname))
			return 0;
		dbf->MoveFirst();
		Record *tmp=new Record;
		while(1) {
			if(!dbf->GetNext(tmp))
				break;
			this->in_pipe->Insert(tmp);
			tmp=new Record;
		}

		if(!dbf->Close())
			return 0;
		delete tmp;
		delete dbf;
	}

	this->in_pipe->ShutDown();
	return 1;
}

int SortedHelper :: writeback(int flag)
{
	if(flag) {
		Record *tmp=new Record;
		SortedFile *sorted=new SortedFile(this->f_info->s_info,
							tmp_name, 1);
		if(!sorted->Create())
			return 0;
		while(1) {
			int stat=this->out_pipe->Remove(tmp);
			if(!stat)
				break;

			sorted->Add(tmp);
			delete tmp;
			tmp=new Record;
		}
		if(!sorted->Close())
			return 0;
		delete tmp;
		delete sorted;
	} else {
		this->f_info->file->AddPage(this->f_info->pg, this->curr_pg++);
		this->f_info->pg->EmptyItOut();
	}
	return 1;
}

int SortedHelper :: unset_dirty(int pseudo)
{
	this->dirty=0;

	if(!pseudo) {
		if(!this->feed())
			return 0;
	}

	if(!this->writeback(!pseudo))
		return 0;

	if(!pseudo) {
		if(!this->reboot())
			return 0;

	}

	return 1;
}

void SortedHelper :: Add(Record *placeholder, int internal)
{
	if(internal) {
		while(1) {
			int stat=this->f_info->pg->Append(placeholder);
			if(!stat) {
				if(!this->writeback(0)) {
					std :: cerr
						<< "Error in writing back!\n";
					_exit(-1);
				}
			} else {
				break;
			}
		}
	} else {
		this->in_pipe->Insert(placeholder);
	}
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

int SortedHelper :: reboot()
{
	if(!this->f_info->file->Close())
		return 0;

	struct stat buf;
	if(stat("bin/runs.bin", &buf)==0) {
		if(unlink("bin/runs.bin")<0)
			return 0;
	}

	if(!stat(tmp_name, &buf)) {
		if(rename(tmp_name, this->f_info->fname)<0) {
			std :: cerr << "Error in renaming!"
				<< strerror(errno) << std :: endl;
			_exit(-1);
		}
	}

	if(!this->f_info->file->Open(1, this->f_info->fname))
		return 0;
}
