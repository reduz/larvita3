//
// C++ Implementation: gui_timer
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gui_timer.h"
#include "os/time.h"

GUI::TimerID GUI_Timer::create_timer( Method p_callback, int p_interval_msec ) {
	
	int idx=-1;
	
	for (int i=0;i<MAX_TIMERS;i++) {
		
		if (!timers[i].active) {
			
			idx=i;
			break;
		}
	}
			
	if (idx==-1) 
		return GUI::INVALID_TIMER_ID;
	
	TimeStruct&t=timers[idx];
	
	t.active=true;
	t.interval=p_interval_msec;
	t.remaining=p_interval_msec;
	t.callback=p_callback;
	
	active_timers[active_timer_count++]=idx;
	
	return idx;
}
void GUI_Timer::remove_timer(GUI::TimerID p_timer) {
	
	for (int i=0;i<active_timer_count;i++) {
		
		int idx=active_timers[i];
		ERR_CONTINUE(idx<0 || idx>=MAX_TIMERS);
		
		if (idx!=p_timer)
			continue;
		
		timers[idx].active=false;
		active_timers[i]=active_timers[active_timer_count-1];
		active_timer_count--;
		return;
	}
	
}
int GUI_Timer::get_interval(GUI::TimerID p_timer) {
	
	ERR_FAIL_INDEX_V( p_timer, MAX_TIMERS, -1 );
	ERR_FAIL_COND_V( !timers[p_timer].active, -1 );
	
	return timers[p_timer].interval;	
}
void GUI_Timer::change_timer_interval(GUI::TimerID p_timer,int p_interval_msec) {
	
	ERR_FAIL_INDEX( p_timer, MAX_TIMERS);
	ERR_FAIL_COND( !timers[p_timer].active );
	
	timers[p_timer].interval=p_interval_msec;
}
unsigned long int GUI_Timer::get_tick_ms() {
	
	return OSTime::get_singleton()->get_ticks_usec()/1000;
}

void GUI_Timer::poll() {
	
	Uint32 ticks_new = get_tick_ms();
	Uint32 diff=ticks_new-ticks_last;
	if (diff==0)
		return;
		
	for (int i=0;i<active_timer_count;i++) {
		
		int idx=active_timers[i];
		ERR_CONTINUE(idx<0 || idx>=MAX_TIMERS);
		
		TimeStruct &t=timers[idx];
		
		ERR_CONTINUE(!t.active);
		
		Uint32 tick_left=diff;
		
		while (tick_left) {
			
			Uint32 dec=MIN( tick_left, t.remaining );
			t.remaining-=dec;
			tick_left-=dec;
			
			if (t.remaining==0) {
				
				t.remaining=t.interval;
				t.callback.call();
			}
		}
		
		
	}
	
	ticks_last=ticks_new;
}

GUI_Timer::GUI_Timer() {
	
	active_timer_count=0;
	ticks_last=get_tick_ms();
	
}


GUI_Timer::~GUI_Timer()
{
}


