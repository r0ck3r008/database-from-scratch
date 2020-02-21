#include"dbfile.h"

DBFile :: DBFile()
{
	this->file=new File;
	this->pg=new Page;
}

DBFile :: ~DBFile()
{
	switch(this->type) {
	case Sorted:
		break;
	case Tree:
		break;
	default:
		delete this->heap;
	}
	delete file;
	delete pg;
}

int DBFile :: Create(const char *fname, fType type, SortInfo *info)
{
	int ret=1;

	switch(type) {
	case Sorted:
		this->file->set_type(Sorted);
		break;
	case Tree:
		this->file->set_type(Tree);
		break;
	default:
		if(type!=Heap)
			std :: cerr << "Unrecognized type: " << type
				<< ". Falling Back to Heap!\n";

		this->heap=new HeapFile(this->file, this->pg);
		this->file->set_type(Heap);
	}

	if(!this->file->Open(0, fname))
		ret=0;
	this->type=type;
	return ret;
}

int DBFile :: Open(const char *fname)
{
	if(!this->file->Open(1, fname))
		return 0;
	this->type=this->file->get_type();

	switch(this->type) {
	case Sorted:
		break;
	case Tree:
		break;
	default:
		this->heap=new HeapFile(this->file, this->pg);
	}
	return 1;
}

void DBFile :: Add(Record *in)
{
	switch(this->type) {
	case Sorted:
		break;
	case Tree:
		break;
	default:
		this->heap->Add(in);
	}
}

void DBFile :: Load(Schema *sch, const char *fname)
{
	switch(this->type) {
	case Sorted:
		break;
	case Tree:
		break;
	default:
		//default action is heap file
		this->heap->Load(sch, fname);
	}
}

void DBFile :: MoveFirst()
{
	switch(this->type) {
	case Sorted:
		break;
	case Tree:
		break;
	default:
		this->heap->MoveFirst();
	}
}

int DBFile :: GetNext(Record *placeholder)
{
	int ret=1;
	switch(this->type) {
	case Sorted:
		break;
	case Tree:
		break;
	default:
		if(!this->heap->GetNext(placeholder))
			ret=0;
	}

	return ret;
}

int DBFile :: GetNext(Record *placeholder, CNF *cnf, Record *literal)
{
	int ret=1;
	switch(this->type) {
	case Sorted:
		break;
	case Tree:
		break;
	default:
		if(!this->heap->GetNext(placeholder, cnf, literal))
			ret=0;
	}

	return ret;
}

int DBFile :: Close()
{
	int ret=1;
	switch(this->type) {
	case Sorted:
		break;
	case Tree:
		break;
	default:
		if(!this->heap->Close())
			ret=0;
	}

	return ret;
}
