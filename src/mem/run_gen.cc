#include<queue>
#include<string.h>
#include<unistd.h>

#include"run_gen.h"

RunGen :: RunGen(Pipe *in_pipe, int run_len, OrderMaker *order)
{
	this->in_pipe=in_pipe;
	this->run_len=run_len;
	this->size_curr_run=0;
	this->buf=NULL;
	this->order=order;
	if(!this->setup_dbf()) {
		std :: cerr << "Error in setting up DBFile!\n";
		_exit(-1);
	}
}

RunGen :: ~RunGen()
{
	delete this->dbf;
}

int RunGen :: setup_dbf()
{
	//first instance
	dbf=new DBFile;
	if(!this->dbf->Create("bin/runs.bin", heap, NULL)) {
		std :: cerr << "Error in creating run dbfile!\n";
		return 0;
	}

	return 1;
}

void RunGen :: write(std :: queue <Record *> *que)
{
	Record *rec;
	int count=0;
	while(que->size()!=0) {
		rec=que->front();
		que->pop();

		this->dbf->Add(rec);
		count++;
	}

	this->rec_sizes.push_back(count);
}

int RunGen :: fetch_rec(Record **rec)
{
	*rec=new Record;
	if(this->buf!=NULL) {
		*rec=this->buf;
		this->buf=NULL;
		this->size_curr_run+=(*rec)->get_size();
		goto exit;
	} else {
		if(!this->in_pipe->Remove(*rec))
			return -1;
		this->size_curr_run+=(*rec)->get_size();
	}

	if(this->size_curr_run>=(PAGE_SIZE*this->run_len)) {
		//reset run
		this->buf=*rec;
		this->size_curr_run=0;
		return 1;
	}

exit:
	return 0;
}

std :: vector <int> *RunGen :: generator()
{
	int flag=1;
	while(flag) {
		Record **rec;
		std :: vector <Record *> rec_vec;
		int rec_count=0;
		int stat;
		while(1) {
			rec=new Record *;
			stat=this->fetch_rec(rec);
			if(stat)
				break;

			rec_vec.push_back(*rec);
			rec_count++;
//			delete rec;
		}
		if(stat==-1) {
			std :: cerr << "End of records!\n";
			flag=0;
		}
		//sort
		Tournament *tour=new Tournament(rec_count, this->order);
		for(int i=0; i<rec_count; i++) {
			Record *rec_feed=rec_vec[i];
			if(!(tour->feed(rec_feed))) {
				std :: cerr << "Error feeding record #"
					<< i << std :: endl;
				break;
			}
		}
		std :: queue <Record *> *rec_queue=tour->flush();
		this->write(rec_queue);
		delete tour;
	}
	if(!this->dbf->Close()) {
		std :: cerr << "Error in closing dbf!\n";
		_exit(-1);
	}

	return &(this->rec_sizes);
}

// Note:
// 1. fetch records using the fetch function and store in the class variable of
//    a queue;
// 2. the fetch function takes care of the how many records are fetched and till
// 3. put the queue through the tournament tree
// 4. write the current run and redo
// 5. write to the file etc. is done using a dbfile instance
