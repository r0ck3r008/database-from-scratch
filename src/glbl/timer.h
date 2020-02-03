#ifndef TIMER_H
#define TIMER_H

#include<sys/time.h>

class timer
{
private:
	struct timeval start;
	struct timeval end;
	struct timeval diff;
public:
	void start_timer();
	void stop_timer();
	struct timeval get_tt();
};


#endif
