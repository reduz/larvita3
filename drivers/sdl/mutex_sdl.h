//
// C++ Interface: mutex_sdl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MUTEX_SDL_H
#define MUTEX_SDL_H

#ifdef SDL_ENABLED

#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "os/mutex.h"
/**
	@author ,,, <red@lunatea>
*/
class Mutex_SDL : public Mutex {
	static Mutex* create_func_SDL();	

	SDL_mutex *mutex;
public:

	virtual void lock(); 
	virtual void unlock(); 
	virtual bool try_lock(); 
	
	static void set_as_default();
	
	Mutex_SDL();	
	~Mutex_SDL();

};

#endif
#endif
