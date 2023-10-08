//
// C++ Interface: time_posix
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TIME_POSIX_H
#define TIME_POSIX_H

#include "os/time.h"

#ifdef POSIX_ENABLED

/**
	@author ,,, <red@lunatea>
*/
class TimePosix : public OSTime {

	Uint64 ticks_start;

public:

	virtual void get_date(int& r_year,Month& r_month,int& r_day,Weekday& r_weekday,bool& r_dst) const;
	virtual void get_time(int& r_hour, int& r_min,int& r_sec) const;
	virtual void delay_usec(Uint32 p_usec) const; 
	virtual Uint64 get_ticks_usec() const;

	TimePosix();	
	~TimePosix();

};

#endif
#endif
