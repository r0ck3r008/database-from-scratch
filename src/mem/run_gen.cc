#include<queue>
#include<string.h>
#include<unistd.h>

#include"run_gen.h"

RunGen :: RunGen(Pipe *in_pipe, int run_len, OrderMaker *order, char *run_file)
{
	this->in_pipe=in_pipe;
	this->run_len=run_len;
	this->size_curr_run=0;
	this->buf=NULL;
	this->comp=new struct comparator((void *)order, NULL, 0);
	this->run_file=run_file;
	if(!this->setup_dbf()) {
		std :: cerr << "Error in setting up DBFile!\n";
		_exit(-1);
	}
}

RunGen :: ~RunGen()
{
	delete this->comp;
	delete this->dbf;
}

int RunGen :: setup_dbf()
{
	//first instance
	dbf=new DBFile;
	if(!this->dbf->Create(this->run_file, Heap, NULL)) {
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

//TODO
//Make the generator return a const reference, &, so that the vector is not re
//allocated in the memoty and acquires space all over again!
std :: vector <int> *RunGen :: generator()
{
	int flag=1;
	int count=0;
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
			std :: cerr << "End of records!" << "Records: "
				<< count + rec_count << std :: endl;
			flag=0;
		}
		count+=rec_count;
		//sort
		Tournament *tour=new Tournament(rec_count, this->comp);
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
