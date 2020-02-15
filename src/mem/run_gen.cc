#include<queue>
#include<string.h>
#include<unistd.h>

#include"run_gen.h"

RunGen :: RunGen(Pipe *in_pipe, int run_len, OrderMaker *order)
{
	this->in_pipe=in_pipe;
	this->run_len=run_len;
	this->comp=new struct comparator(NULL, NULL, (void *)order, NULL, 0);
	this->dbf=NULL;
	this->size_curr_run=0;
	this->buf=NULL;
}

RunGen :: ~RunGen()
{
	if(!this->dbf->Close())
		_exit(-1);
	delete this->dbf;
	delete this->comp;
}

int RunGen :: setup_dbf()
{
	//first instance
	dbf=new DBFile;
	if(!this->dbf->Create("runs.bin", heap, NULL)) {
		std :: cerr << "Error in creating run dbfile!\n";
		return 0;
	}

	return 1;
}

void RunGen :: write(std :: queue <Record *> que)
{
	if(this->dbf==NULL)
		if(!this->setup_dbf())
			return;

	Record *rec;
	while(que.size()!=0) {
		rec=que.front();
		que.pop();

		this->dbf->Add(rec);
		delete rec;
	}
}

int RunGen :: fetch_rec(Record **rec)
{
	if(this->buf!=NULL) {
		rec=this->buf;
		this->buf=NULL;
		this->size_curr_run+=(*rec)->get_size();
		goto exit;
	} else {
		*rec=new Record;
		if(!this->in_pipe->Remove(*rec))
			return -1;
		this->size_curr_run+=(*rec)->get_size();
	}

	if(this->size_curr_run>=(PAGE_SIZE*this->run_len)) {
		//reset run
		this->buf=rec;
		this->size_curr_run=0;
		return 1;
	}

exit:
	return 0;
}

void RunGen :: generator()
{
	Record **rec;
	int stat;
	while(1) {
		std :: vector <Record *> rec_vec;
		while(1) {
			rec=new Record *;
			stat=this->fetch_rec(rec);
			if(stat)
				break;

			rec_vec.push_back(*rec);
			delete rec;
		}
		if(stat==-1) {
			std :: cerr << "End of records!\n";
			break;
		}
		//sort
		Tournament *tour=new Tournament(rec_vec.size(), this->comp);
		for(int i=0; i<(rec_vec.size()); i++) {
			Record *rec_feed=rec_vec[i];
			if(!(tour->feed(rec_feed))) {
				std :: cerr << "Error feeding record #"
					<< i << std :: endl;
				break;
			}
		}
		std :: queue <Record *> rec_queue=tour->flush();
		this->write(rec_queue);
		delete tour;
	}
}

// Note:
// 1. fetch records using the fetch function and store in the class variable of
//    a queue;
// 2. the fetch function takes care of the how many records are fetched and till
// 3. put the queue through the tournament tree
// 4. write the current run and redo
// 5. write to the file etc. is done using a dbfile instance
