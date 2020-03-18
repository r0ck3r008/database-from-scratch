#include<string.h>
#include<unistd.h>
#include<vector>

#include"join.h"

//TODO
//If the join can also take Schema as input, we can actually join the two
//records and create a real tuple.

void sort_merge(struct join_args *arg)
{
	Pipe out_pipeL(100);
	Pipe out_pipeR(100);
	BigQ bq_l(arg->in_pipe, &out_pipeL, arg->o_left, 8);
	BigQ bq_r(arg->in_pipeR, &out_pipeR, arg->o_right, 8);
	struct comparator comp((void *)arg->o_left, (void *)arg->o_right, 1);

	Record tmp1, tmp2;
	int stat1=out_pipeL.Remove(&tmp1);
	int stat2=out_pipeR.Remove(&tmp2);
	while(1) {
		comp.rec1=&tmp1;
		comp.rec2=&tmp2;
		int stat=Compare(&comp);
		if(stat<1) {
			if(!stat) {
				Record tmp;
				tmp.MergeRecords(&tmp1, &tmp2, arg->nattsL,
								arg->nattsR,
								arg->atts,
								arg->nattsT,
								arg->nattsL);
				//merge records and push to out pipe
				arg->out_pipe->Insert(&tmp);
				explicit_bzero(&tmp, sizeof(Record));
				explicit_bzero(&tmp1, sizeof(Record));
			}
			//move left pointer
			explicit_bzero(&tmp1, sizeof(Record));
			if(stat1)
				stat1=out_pipeL.Remove(&tmp1);
		} else {
			//move right pointer
			explicit_bzero(&tmp2, sizeof(Record));
			if(stat2)
				stat2=out_pipeR.Remove(&tmp2);
		}

		if(!stat1 || !stat2)
			break;
	}

	arg->out_pipe->ShutDown();
}

void nested_loop(struct join_args *arg)
{
	struct comparator comp((void *)arg->literal, (void *)arg->cnf, 2);
	vector<Record> vec;
	Record tmp1;
	int flag=1;

	while(1) {
		int stat1=arg->in_pipe->Remove(&tmp1);
		if(!stat1)
			break;
		int curr_num= (vec.size()!=0) ? (vec.size()) : 1;
		while(curr_num) {
			Record tmp2;
			if(flag) {
				int stat2=arg->in_pipeR->Remove(&tmp2);
				if(!stat2) {
					flag=0;
					break;
				} else {
					vec.push_back(tmp2);
				}
			} else {
				tmp2=vec[curr_num--];
			}
			comp.rec1=&tmp1;
			Record *tmp_ptr=&tmp2;
			comp.rec2=tmp_ptr;
			if(Compare(&comp)) {
				Record tmp;
				tmp.MergeRecords(&tmp1, &tmp2, arg->nattsL,
						arg->nattsR, arg->atts,
						arg->nattsT, arg->nattsS);
				arg->out_pipe->Insert(&tmp1);
				explicit_bzero(&tmp, sizeof(Record));
			}
		}
		explicit_bzero(&tmp1, sizeof(Record));
	}

	arg->out_pipe->ShutDown();
}

void *join_thr(void *a)
{
	struct join_args *arg=(struct join_args *)a;
	//make OrderMakers
	OrderMaker o_left, o_right;
	int stat=arg->cnf->GetSortOrders(o_left, o_right);
	if(!stat) {
		nested_loop(arg);
	} else {
		arg->o_left=&o_left;
		arg->o_right=&o_right;
		sort_merge(arg);
	}

	pthread_exit(NULL);
}

Join :: Join()
{
	this->arg=new struct join_args;
}

Join :: ~Join()
{
	delete[] this->arg->atts;
	delete this->arg;
}

void Join :: Run(Pipe *in_pipeL, Pipe *in_pipeR, Pipe *out_pipe, CNF *cnf,
			Record *literal, Schema *schL, Schema *schR)
{
	this->arg->in_pipe=in_pipeL;
	this->arg->in_pipeR=in_pipeR;
	this->arg->out_pipe=out_pipe;
	this->arg->cnf=cnf;
	this->arg->literal=literal;
	this->arg->nattsL=schL->GetNumAtts();
	this->arg->nattsR=schR->GetNumAtts();
	this->arg->nattsT=(this->arg->nattsL) + (this->arg->nattsR);
	this->arg->nattsS=this->arg->nattsL-1;
	int *atts=new int[this->arg->nattsT];
	for(int i=0, j=0; i<=this->arg->nattsT; i++) {
		if(i<this->arg->nattsL)
			atts[i]=j++;
		else if(i==this->arg->nattsL)
			j=0;
		else
			atts[i-1]=j++;
	}
	this->arg->atts=atts;

	int stat=pthread_create(&(this->tid), NULL, join_thr,
					(void *)this->arg);
	if(stat) {
		std :: cerr << "Error in creating the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void Join :: WaitUntilDone()
{
	int stat=pthread_join(this->tid, NULL);

	if(stat) {
		std :: cerr << "Error in joining to the thread: "
			<< strerror(stat) << std :: endl;
		_exit(-1);
	}
}

void Join :: Use_n_Pages(int n)
{
	this->max_pgs=n;
}
