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
	this->query=NULL;
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

int SortedFile :: Close()
{
	if(this->helper->chk_dirty())
		if(!this->helper->unset_dirty(this->pseudo))
			return 0;

	if(!this->f_info->file->Close())
		return 0;

	return 1;
}

int SortedFile :: GetNext (Record *fetchme, CNF *cnf, Record *literal)
{
	if(this->helper->chk_dirty()) {
		if(!this->helper->unset_dirty(pseudo))
			return 0;
	}

	ComparisonEngine comp;

	if (this->query==NULL) {
		// query does not exist
		this->query = new OrderMaker;
		if (query->query_generator(&(this->f_info->s_info->order),
						cnf) > 0) {
			// query generated successfully
			query->Print ();
			if (this->binary_search(fetchme, cnf, literal)) {
				// Found
				return 1;
			} else {
				// binary search fails
				return 0;
			}
		} else {
			//query generated but is empty
			return this->get_next_no_query(fetchme, cnf, literal);
		}
	} else {
		// query exists
		if (this->query->get_num_attrs() == 0) {
			// invalid query
			return this->get_next_no_query(fetchme, cnf, literal);
		} else {
			// valid query
			return this->get_next_query(fetchme, cnf, literal);
		}
	}
}

int SortedFile :: get_next_query(Record *fetchme, CNF *cnf, Record *literal)
{
	ComparisonEngine comp;

	while (this->GetNext(fetchme)) {
		if (!comp.Compare (literal, query, fetchme,
					&(this->f_info->s_info->order))){
			if (comp.Compare (fetchme, literal, cnf)){
				return 1;
			}
		}else {
			break;
		}
	}
	return 0;
}

int SortedFile :: get_next_no_query(Record *fetchme, CNF *cnf, Record *literal)
{
	ComparisonEngine comp;

	while (this->GetNext(fetchme)) {
		if (comp.Compare (fetchme, literal, cnf)){
			return 1;
		}
	}
	return 0;
}

int SortedFile :: binary_search(Record *fetchme, CNF *cnf, Record *literal)
{
	off_t first = this->helper->fetch_curr_pg();
	off_t last = this->f_info->file->GetLength () - 1;
	off_t mid = first;

	Page *page = new Page;

	ComparisonEngine comp;

	while (true) {
		mid = (first + last) / 2;
		this->f_info->file->GetPage(page, mid);
		if (page->GetFirst (fetchme)) {
			if (comp.Compare (literal, this->query, fetchme, &(this->f_info->s_info->order)) <= 0) {
				last = mid - 1;
				if (last <= first) break;
			} else {
				first = mid + 1;
				if (last <= first) break;
			}
		} else {
			break;
		}
	}

	if (comp.Compare (fetchme, literal, cnf)) {
		delete this->f_info->pg;
		this->helper->set_curr_pg(mid);
		this->f_info->pg = page;
		return 1;
	} else {
		delete page;
		return 0;
	}
}
