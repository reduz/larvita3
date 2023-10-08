#ifdef SDL_ENABLED
//
// C++ Interface: timer_sdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PIGUITIMER_SDL_H
#define PIGUITIMER_SDL_H

#include "base/timer.h"
#include <SDL.h>

namespace GUI {

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class TimerSDL : public Timer {
	
	enum {
		
		MAX_TIMERS=8192
	};
	
	struct TimerData {
		
		TimerID id;
		Method method;
		Signal<> callback;
		int interval;
		bool active;
		SDL_TimerID sdl_timer;
		Uint32 last_iter;
		TimerData() { active=false; interval=0; id=-1; last_iter=0; }
		
	};
	
	
	Uint32 iteration;
	TimerData timers[MAX_TIMERS];
	
	static Uint32 timer_callback(Uint32 interval, void *_arg);

public:
	
	enum {
		SDL_TIMER_EVENT_CODE=0xC10C
	};
	
	virtual TimerID create_timer( Method p_callback, int p_interval_msec );
	virtual void remove_timer(TimerID p_timer);
	virtual void change_timer_interval(TimerID p_timer,int p_interval_msec);
	virtual int get_interval(TimerID p_timer);	
	void call(TimerID p_timer);
	unsigned long int get_tick_ms();
	
	void loop_iterate(); //avoids events being called more than once per processing loop
	
	void stop_all();
	TimerSDL();
	
	~TimerSDL();

};

}

#endif

#endif
