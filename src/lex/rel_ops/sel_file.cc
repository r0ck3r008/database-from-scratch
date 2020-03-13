#include<string.h>
#include<unistd.h>

#include"../rel_op.h"

void *run_thr(void *input)
{
	struct thr_args *arg=(struct thr_args *)input;
	char **retval=new char *;
	*retval=new char[256];

	Record *tmp=new Record;
	while(1) {
		if(!arg->dbf->GetNext(tmp, arg->cnf, arg->literal)) {
			sprintf(*retval, "End of records!\n");
			break;
		}
		arg->out_pipe->Insert(tmp);

		delete tmp;
		tmp=new Record;
	}

	delete tmp;
	arg->out_pipe->ShutDown();
	pthread_exit((void **)retval);
}

SelectFile :: SelectFile()
{
	this->arg=new struct thr_args;
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
	int stat=pthread_create(&(this->tid), NULL, run_thr, NULL);
	if(!stat) {
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
	void **retval;
	int stat=pthread_join(this->tid, retval);
	if(!stat) {
		std :: cerr << "Error in joining to the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	} else {
		std :: cout << "Thread Exited Successfully: "
			<< *(char **)retval << std :: endl;
	}

	delete[] *(char **)retval;
	delete (char **)retval;
}