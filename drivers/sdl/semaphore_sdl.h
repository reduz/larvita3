//
// C++ Interface: semaphore_sdl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SEMAPHORE_SDL_H
#define SEMAPHORE_SDL_H

#ifdef SDL_ENABLED

#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"
#include "os/semaphore.h"
/**
	@author ,,, <red@lunatea>
*/
class Semaphore_SDL : public Semaphore {
	static Semaphore* create_func_SDL();	

	SDL_sem *semaphore;
public:

	virtual Error wait();
	virtual Error try_wait();
	virtual Error wait_with_timeout(int p_msec);
	virtual Error post();
	virtual int get() const;
	
	static void set_as_default();
	
	Semaphore_SDL();	
	~Semaphore_SDL();
};


#endif
#endif
