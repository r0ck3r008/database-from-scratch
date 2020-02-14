#include"run_gen.h"

RunGen :: RunGen(Pipe *in_pipe, int run_len)
{
	this->in_pipe=in_pipe;
	this->run_len=run_len;
	this->tour=new Tournament <Record>(run_len/4);
}

RunGen :: ~RunGen()
{
	delete this->tour;
}
