#include<iostream>

#include"timer.h"

void timer :: start_timer()
{
	gettimeofday(&this->start, NULL);
}

void timer :: stop_timer()
{
	gettimeofday(&this->end, NULL);
}

struct timeval timer :: get_tt()
{
	timersub(&this->end, &this->start, &this->diff);
	return this->diff;
}
