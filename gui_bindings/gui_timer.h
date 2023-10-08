//
// C++ Interface: gui_timer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GUI_TIMER_H
#define GUI_TIMER_H

#include "base/timer.h"


/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class GUI_Timer : public GUI::Timer {
	
	enum {
		MAX_TIMERS=2048
	};
	
	struct TimeStruct {
		
		Method callback;
		int interval;
		int remaining;
		bool active;
		
		TimeStruct() { remaining=0; active=false; }
	};
	
	
	TimeStruct timers[MAX_TIMERS];
	int active_timers[MAX_TIMERS];
	int active_timer_count;
	
	Uint32 ticks_last;
public:
	
	virtual GUI::TimerID create_timer( Method p_callback, int p_interval_msec );
	virtual void remove_timer(GUI::TimerID p_timer);
	virtual int get_interval(GUI::TimerID p_timer);
	virtual void change_timer_interval(GUI::TimerID p_timer,int p_interval_msec);
	virtual unsigned long int get_tick_ms(); /* get a millisecond tick, for delta times and not synced to clock */	
	
	void poll();
	
	GUI_Timer();	
	~GUI_Timer();

};

#endif
