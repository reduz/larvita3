#ifdef SDL_ENABLED
//
// C++ Implementation: timer_sdl
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "timer_sdl.h"
#include <stdio.h>
namespace GUI {


Uint32 TimerSDL::timer_callback(Uint32 interval, void *_arg) {
	
	TimerData *td=(TimerData*)_arg;
	
	SDL_Event ev;
	ev.type=SDL_USEREVENT;
	ev.user.type=SDL_USEREVENT;
	ev.user.code=SDL_TIMER_EVENT_CODE;
	ev.user.data1=&td->id;

	SDL_PushEvent(&ev);
	return td->interval;
}

	
TimerID TimerSDL::create_timer( Method p_callback, int p_interval_msec ) {
	
	TimerID id=-1;
	for (int i=0;i<MAX_TIMERS;i++) {
		
		if (!timers[i].active) {
			id=i;
			break;
		}
	}
	
	if (id==-1)
		return id;
	
	timers[id].sdl_timer=SDL_AddTimer(p_interval_msec, timer_callback, &timers[id]);	
//	printf("adding callback.., res is %p\n",timers[id].sdl_timer);
	if (!timers[id].sdl_timer)
		return -1;
	timers[id].method=p_callback;
	timers[id].active=true;
	timers[id].callback.connect(p_callback);
	timers[id].id=id;
	timers[id].interval=p_interval_msec;
		
	return id;
}
void TimerSDL::remove_timer(TimerID p_timer) {
	
	if (p_timer<0 || p_timer>=MAX_TIMERS)
		return;
	
	if (!timers[p_timer].active)
		return;
	
	timers[p_timer].callback.disconnect( timers[p_timer].method );
	SDL_RemoveTimer(timers[p_timer].sdl_timer);
	timers[p_timer].active=false;

	
}
void TimerSDL::change_timer_interval(TimerID p_timer,int p_interval_msec) {
	
	if (p_timer<0 || p_timer>=MAX_TIMERS)
		return;
	
	if (!timers[p_timer].active)
		return;
	
	timers[p_timer].interval=p_interval_msec;	
	
}
	
int TimerSDL::get_interval(TimerID p_timer) {
	
	if (p_timer<0 || p_timer>=MAX_TIMERS)
		return -1;
	
	if (!timers[p_timer].active)
		return -1;
	
	return timers[p_timer].interval;
	
}
	
void TimerSDL::call(TimerID p_timer) {
	
	if (p_timer<0 || p_timer>=MAX_TIMERS)
		return;
	
	if (!timers[p_timer].active)
		return;
	
	if (timers[p_timer].last_iter==iteration)
		return;
	
	timers[p_timer].last_iter=iteration;
	timers[p_timer].callback.call();
	
}

void TimerSDL::loop_iterate() {
	
	iteration++;
}

unsigned long int TimerSDL::get_tick_ms() {
	
	return SDL_GetTicks();	
}

TimerSDL::TimerSDL(){
	
	
}

void TimerSDL::stop_all() {
	
	for (int i=0;i<MAX_TIMERS;i++) {
		if (timers[i].active)
			remove_timer( i );
		
	}
	
}
TimerSDL::~TimerSDL() {
	
	stop_all();		
}


}
#endif
