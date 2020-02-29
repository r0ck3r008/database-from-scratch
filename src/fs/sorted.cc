#include<unistd.h>

#include"sorted.h"

file_info :: file_info(struct SortInfo *s_info, const char *fname, File *file,
			Page *pg)
{
	this->s_info=s_info;
	this->fname=fname;
	this->file=file;
	this->pg=pg;
}

SortedFile :: SortedFile(struct SortInfo *s_info, const char *fname)
{
	this->head=NULL;
	File *file=new File;
	Page *pg=new Page;
	this->f_info=new file_info(s_info, fname, file, pg);
	this->helper=new SortedHelper(this->f_info);
}

SortedFile :: ~SortedFile()
{
	delete this->f_info->file;
	delete this->helper;
	delete this->f_info->pg;
}

int SortedFile :: fetch(int pg_num)
{
	if(this->helper->chk_dirty())
		if(!this->helper->unset_dirty())
			return 0;

	int curr_len=this->f_info->file->GetLength();
	if(pg_num<curr_len) {
		this->f_info->pg->EmptyItOut();
		this->f_info->file->GetPage(this->f_info->pg, pg_num);
		this->helper->set_curr_pg(pg_num);
		return 1;
	} else {
		return 0;
	}
}

int SortedFile :: Create()
{
	if(!this->f_info->file->Open(0, this->f_info->fname))
		return 0;
	this->helper->set_create(1);
	this->f_info->file->set_type(Sorted);
	this->f_info->file->set_info(this->f_info->s_info);

	return 1;
}

int SortedFile :: Open()
{
	if(!this->f_info->file->Open(1, this->f_info->fname))
		return 0;

	if(!this->f_info->file->GetLength())
		this->helper->set_create(1);
	else
		this->helper->set_create(0);
	this->f_info->s_info=this->f_info->file->get_info();
	return 1;
}

void SortedFile :: Add(Record *placeholder)
{
	if(!this->helper->chk_dirty())
		this->helper->set_dirty();

	if(this->helper->get_create()) {
		off_t curr_len=this->f_info->file->GetLength();
		int curr_pg=this->helper->fetch_curr_pg();
		if(curr_len==0 || curr_pg==curr_len-2) {
			//this is on latest page
			//check size and writeback if necessary
			if(this->f_info->pg->get_curr_size() +
					placeholder->get_size() > PAGE_SIZE) {
				this->f_info->file->AddPage(this->f_info->pg,
								curr_pg+1);
				this->f_info->pg->EmptyItOut();
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
		this->helper->Add(placeholder);
	}
}

void SortedFile :: Load(Schema *sch, const char *fname)
{
	FILE *f=NULL;
	if((f=fopen(fname, "r"))==NULL) {
		std :: cerr << "Error in opening the file " << fname
			<< std :: endl;
		_exit(-1);
	}

	Record *tmp=new Record;
	while(1) {
		int stat=tmp->SuckNextRecord(sch, f);
		if(!stat) {
			if(!feof(f))
				std :: cerr << "Error in reading the file "
					<< fname << std :: endl;
			else
				std :: cout << "EOF!\n";
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
	if(this->helper->chk_dirty())
		if(!this->helper->unset_dirty()) {
			std :: cerr << "Error in unsetting dirty!\n";
			_exit(-1);
		}

	int curr_len=this->f_info->file->GetLength();
	if(!this->fetch(0)) {
		std :: cerr << "Error in fetching page 0\n";
		_exit(-1);
	}
}

int SortedFile :: GetNext(Record *placeholder)
{
	if(this->helper->chk_dirty())
		this->MoveFirst();
	int curr_len=this->f_info->file->GetLength();
	int curr_pg=this->helper->fetch_curr_pg();

	int ret=1;
	while(ret) {
		int stat=this->f_info->pg->GetFirst(placeholder);
		if(!stat) {
			if(curr_pg!=curr_len-2)
				if(!this->fetch(curr_pg+1))
					ret=0;
			else
				ret=0;
		} else {
			break;
		}
	}

	this->head=placeholder;

	return ret;
}

int SortedFile :: GetNext(Record *placeholder, CNF *cnf, Record *literal)
{
	int ret=1;

	return ret;
}

int SortedFile :: Close()
{
	if(this->helper->chk_dirty())
		if(!this->helper->unset_dirty())
			return 0;

	if(!this->f_info->file->Close())
		return 0;
	return 1;
}
