#include"run_gen.h"

RunGen :: RunGen(Pipe *in_pipe, int run_len, OrderMaker *order)
{
	this->in_pipe=in_pipe;
	this->run_len=run_len;
	this->order=order;
	this->comp=new struct comparator(NULL, NULL, (void *)order, NULL, 0);
	this->tour=new Tournament <Record> (run_len/4, comp);
}

RunGen :: ~RunGen()
{
	delete this->comp;
	delete this->tour;
}
