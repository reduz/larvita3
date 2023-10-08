#ifdef WINDOWS_ENABLED

#ifndef TIME_WINDOWS_H
#define TIME_WINDOWS_H

#include "os/time.h"

class TimeWindows : public OSTime {

	Uint64 ticks_start;
	Uint64 ticks_per_second;

public:

	virtual void get_date(int& r_year,Month& r_month,int& r_day,Weekday& r_weekday,bool& r_dst) const;
	virtual void get_time(int& r_hour, int& r_min,int& r_sec) const;
	virtual void delay_usec(Uint32 p_usec) const;
	virtual Uint64 get_ticks_usec() const;

	TimeWindows();
	~TimeWindows();
};


#endif

#endif // WINDOWS_ENABLED
