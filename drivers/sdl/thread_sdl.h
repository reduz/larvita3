//
// C++ Interface: thread_sdl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef THREAD_SDL_H
#define THREAD_SDL_H

#ifdef SDL_ENABLED

#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "os/thread.h"

/**
	@author ,,, <red@lunatea>
*/
class Thread_SDL : public Thread {

	SDL_Thread *thread;
	Method call_method;

	static int _initiate_thread(void*);	

	static Thread* create_func_SDL(Method,const Settings&);
	static Uint32 get_thread_ID_func_SDL();
	static void wait_to_finish_func_SDL(Thread*);	
public:

	static void set_as_default();

	Thread_SDL();
	~Thread_SDL();

};

#endif
#endif
