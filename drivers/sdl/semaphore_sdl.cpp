//
// C++ Implementation: semaphore_sdl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "semaphore_sdl.h"
#ifdef SDL_ENABLED
#include "error_macros.h"
#include "os/memory.h"

Error Semaphore_SDL::wait() {

	int err=SDL_SemWait(semaphore);
	if (err==-1)
		return ERR_CANT_AQUIRE_RESOURCE;
		
	return OK;
}
Error Semaphore_SDL::try_wait() {

	int err=SDL_SemTryWait(semaphore);
	if (err==-1)
		return ERR_CANT_AQUIRE_RESOURCE;
	else if (err==SDL_MUTEX_TIMEDOUT)
		return ERR_TIMEOUT;
		
	return OK;

}
Error Semaphore_SDL::wait_with_timeout(int p_msec) {

	int err=SDL_SemWaitTimeout(semaphore,p_msec);
	if (err==-1)
		return ERR_CANT_AQUIRE_RESOURCE;
	else if (err==SDL_MUTEX_TIMEDOUT)
		return ERR_TIMEOUT;
		
	return OK;

}
Error Semaphore_SDL::post() {

	int err=SDL_SemPost(semaphore);
	
	return err?ERR_CANT_AQUIRE_RESOURCE:OK;
}
int Semaphore_SDL::get() const {
	
	return SDL_SemValue(semaphore);
}

Semaphore* Semaphore_SDL::create_func_SDL() {

	return memnew( Semaphore_SDL );
}

void Semaphore_SDL::set_as_default() {

	create_func=create_func_SDL;
}

Semaphore_SDL::Semaphore_SDL() {

	semaphore = SDL_CreateSemaphore(0);
}

Semaphore_SDL::~Semaphore_SDL() {

	SDL_DestroySemaphore(semaphore);
}

#endif
