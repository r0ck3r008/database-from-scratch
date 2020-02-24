#include"dbfile.h"

DBFile :: DBFile()
{
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
}

int DBFile :: get_type(const char *fname)
{
	File *f=new File;
	if(!f->Open(1, fname))
		return 0;

	this->type=f->get_type();

	if(!f->Close())
		return 0;
	delete f;

	return 1;
}

int DBFile :: Create(const char *fname, fType type, SortInfo *info)
{
	int ret=1;

	switch(type) {
	case Sorted:
		break;
	case Tree:
		break;
	default:
		if(type!=Heap)
			std :: cerr << "Unrecognized type: " << type
				<< ". Falling Back to Heap!\n";

		this->heap=new HeapFile;
		if(!this->heap->Create(fname))
			ret=0;
	}

	this->type=type;
	return ret;
}

int DBFile :: Open(const char *fname)
{
	int ret=1;
	if(!this->get_type(fname))
		std :: cerr << "Error in getting type of: "
			<< fname <<
			". Any further execution is unpridictiable!\n";

	switch(this->type) {
	case Sorted:
		break;
	case Tree:
		break;
	default:
		this->heap=new HeapFile;
		if(!this->heap->Open(fname))
			ret=0;
	}

	return ret;
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
