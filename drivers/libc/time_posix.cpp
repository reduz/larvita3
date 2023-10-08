//
// C++ Implementation: time_posix
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "time_posix.h"
#ifdef POSIX_ENABLED
#include <time.h> 
#include <sys/time.h>
#include <unistd.h>


void TimePosix::get_date(int& r_year,Month& r_month,int& r_day,Weekday& r_weekday,bool& r_dst) const {
	
	time_t t=time(NULL);
	struct tm *lt=localtime(&t);
	r_year=lt->tm_year;
	r_month=(Month)lt->tm_mon;
	r_day=lt->tm_mday;
	r_weekday=(Weekday)lt->tm_wday;
	r_dst=lt->tm_isdst;

}
void TimePosix::get_time(int& r_hour, int& r_min,int& r_sec) const {

	time_t t=time(NULL);
	struct tm *lt=localtime(&t);
	r_hour=lt->tm_hour;
	r_min=lt->tm_min;
	r_sec=lt->tm_sec;
}

void TimePosix::delay_usec(Uint32 p_usec) const {

	usleep(p_usec);
}
Uint64 TimePosix::get_ticks_usec() const {

	struct timeval tv_now;
	gettimeofday(&tv_now,NULL);
	
	Uint64 longtime = (Uint64)tv_now.tv_usec + (Uint64)tv_now.tv_sec*1000000L;
	longtime-=ticks_start;
	
	return longtime;

}


TimePosix::TimePosix() {

	ticks_start=0;
	ticks_start=get_ticks_usec();

}


TimePosix::~TimePosix()
{
}

#endif
