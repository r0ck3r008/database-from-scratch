#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include"rel_ops.h"

void get_sum_rec(struct sum_args *arg, int *int_res, double *double_res)
{
	Record tmp;
	while(1) {
		int curr_int_res=0; double curr_double_res=0;
		int stat=arg->in_pipe->Remove(&tmp);
		if(!stat)
			break;
		arg->func->Apply(tmp, curr_int_res, curr_double_res);
		//since one of them is gonna be 0 anyway, why not add both
		//Plus and if statement would make things harder to read anyway
		*int_res+=curr_int_res;
		*double_res+=curr_double_res;

		explicit_bzero(&tmp, sizeof(Record));
	}
}

void *sum_thr(void *a)
{
	struct sum_args *arg=(struct sum_args *)a;

	Type ret_type=((arg->func->get_ret_type()==1) ? (Int) : (Double));
	Schema sch;
	sch.myAtts[0].update("Double", Double);
	sch.numAtts=1;

	int int_res=0; double double_res=0;
	get_sum_rec(arg, &int_res, &double_res);

	char *result=new char[128];
	Record tmp;
	if(ret_type==Int)
		sprintf(result, "%d|", int_res);
	else
		sprintf(result, "%0.lf|", double_res);
	tmp.ComposeRecord(&sch, result);
	arg->out_pipe->Insert(&tmp);

	arg->out_pipe->ShutDown();
	pthread_exit(NULL);
}

Sum :: Sum()
{
	this->args=new struct sum_args;
}

Sum :: ~Sum()
{
	delete this->args;
}

void Sum :: Run(Pipe *in_pipe, Pipe *out_pipe, Function *func)
{
	this->args->in_pipe=in_pipe;
	this->args->out_pipe=out_pipe;
	this->args->func=func;

	int stat=pthread_create(&(this->tid), NULL, sum_thr,
				(void *)this->args);
	if(stat) {
		std :: cerr << "Erorr in creating the thread: "
			<< strerror(stat) << std :: endl;
	}
}

void Sum :: WaitUntilDone()
{
	int stat=pthread_join(this->tid, NULL);
	if(stat) {
		std :: cerr << "Error in joining the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void Sum :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}
