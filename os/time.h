//
// C++ Interface: time
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TIME_H
#define TIME_H


#include "typedefs.h"

/**
	@author ,,, <red@lunatea>
*/
class OSTime {

	static OSTime* singleton;

public:
	static OSTime* get_singleton();
	enum Weekday {
		DAY_SUNDAY,
		DAY_MONDAY,
		DAY_TUESDAY,
		DAY_WEDNESDAY,
		DAY_THURSDAY,
		DAY_FRIDAY,
		DAY_SATURDAY
	};
	
	enum Month {
		MONTH_JANUARY,
		MONTH_FEBRUARY,
		MONTH_MARCH,
		MONTH_APRIL,
		MONTH_MAY,
		MONTH_JUNE,
		MONTH_JULY,
		MONTH_AUGUST,
		MONTH_SEPTEMBER,
		MONTH_OCTOBER,
		MONTH_NOVEMBER,
		MONTH_DECEMBER
	};

	virtual void get_date(int& r_year,Month& r_month,int& r_day,Weekday& r_weekday,bool& r_dst) const=0;
	virtual void get_time(int& r_hour, int& r_min,int& r_sec) const=0;
	virtual void delay_usec(Uint32 p_usec) const=0; 
	virtual Uint64 get_ticks_usec() const=0;

	OSTime();
	virtual ~OSTime();

};

#endif
