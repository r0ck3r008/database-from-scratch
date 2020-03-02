#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
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

SortedFile :: SortedFile(struct SortInfo *s_info, const char *fname, int pseudo)
{
	this->head=NULL;
	File *file=new File;
	Page *pg=new Page;
	this->f_info=new file_info(s_info, fname, file, pg);
	this->helper=new SortedHelper(this->f_info);
	this->pseudo=pseudo;
}

SortedFile :: ~SortedFile()
{
	delete this->f_info->file;
	delete this->helper;
	delete this->f_info->pg;
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

	off_t curr_len=this->f_info->file->GetLength();
	if(!curr_len) {
		this->helper->set_create(1);
	} else {
		this->helper->set_create(0);
		this->helper->fetch(0, this->pseudo);
	}
	this->f_info->s_info=this->f_info->file->get_info();
	return 1;
}

void SortedFile :: Add(Record *placeholder)
{
	if(!this->helper->chk_dirty())
		this->helper->set_dirty(this->pseudo);

	this->helper->Add(placeholder, pseudo);
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
		if(!this->helper->unset_dirty(this->pseudo)) {
			std :: cerr << "Error in unsetting dirty!\n";
			_exit(-1);
		}

	if(!this->helper->fetch(0, this->pseudo)) {
		std :: cerr << "Error in fetching page 0\n";
		_exit(-1);
	}
}

int SortedFile :: GetNext(Record *placeholder)
{
	//GetNext doesnt need to care about which page is currently loaded or if
	//the dirty bit is set, since that is taken care of in MoveToFirst

	off_t curr_pg=this->helper->fetch_curr_pg();
	off_t curr_len=this->f_info->file->GetLength();

	while(1) {
		int stat=this->f_info->pg->GetFirst(placeholder);
		if(!stat) {
			if(curr_pg!=curr_len-2) {
				if(!this->helper->fetch(curr_pg+1, this->pseudo))
					return 0;
			} else {
				return 0;
			}
		} else {
			break;
		}
	}

	this->head=placeholder;

	return 1;
}

int SortedFile :: GetNext(Record *placeholder, CNF *cnf, Record *literal)
{
	int ret=1;

	return ret;
}

int SortedFile :: Close()
{
	if(this->helper->chk_dirty())
		if(!this->helper->unset_dirty(this->pseudo))
			return 0;

	if(!this->f_info->file->Close())
		return 0;

	//TODO
	//Take care of moving the tmp file when someone switches between reading
	//an writing without closing the file
	struct stat buf;
	if(!this->pseudo && !stat(tmp_name, &buf)) {
		if(rename(tmp_name, this->f_info->fname)<0) {
			std :: cerr << "Error in renaming!"
				<< strerror(errno) << std :: endl;
			_exit(-1);
		}
	}
	return 1;
}
