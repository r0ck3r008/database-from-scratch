#include<string.h>
#include<unistd.h>

#include"sel_file.h"

void *file_thr(void *input)
{
	struct file_args *arg=(struct file_args *)input;

	Record tmp;
	while(1) {
		if(!arg->dbf->GetNext(&tmp, arg->cnf, arg->literal))
			break;
		arg->out_pipe->Insert(&tmp);

		explicit_bzero(&tmp, sizeof(Record));
	}

	arg->out_pipe->ShutDown();
	pthread_exit(NULL);
}

SelectFile :: SelectFile()
{
	this->arg=new struct file_args;
}

SelectFile :: ~SelectFile()
{
	delete this->arg;
}

void SelectFile :: Run(DBFile *dbf, Pipe *out_pipe, CNF *cnf, Record *literal)
{
	this->arg->dbf=dbf;
	this->arg->out_pipe=out_pipe;
	this->arg->cnf=cnf;
	this->arg->literal=literal;
	int stat=pthread_create(&(this->tid), NULL, file_thr,
				(void *)this->arg);
	if(stat) {
		std :: cerr << "Error in creating the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void SelectFile :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}

void SelectFile :: WaitUntilDone()
{
	int stat=pthread_join(this->tid, NULL);
	if(stat) {
		std :: cerr << "Error in joining to the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}
