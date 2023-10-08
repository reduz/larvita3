//
// C++ Interface: timer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUITIMER_H
#define PIGUITIMER_H

#include "base/defs.h"
#include "signals/signals.h"


namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Timer{
public:
	
	virtual TimerID create_timer( Method p_callback, int p_interval_msec )=0;
	virtual void remove_timer(TimerID p_timer)=0;
	virtual int get_interval(TimerID p_timer)=0;
	virtual void change_timer_interval(TimerID p_timer,int p_interval_msec)=0;
	virtual unsigned long int get_tick_ms()=0; /* get a millisecond tick, for delta times and not synced to clock */

	
	Timer();
	virtual ~Timer();
};

}

#endif
