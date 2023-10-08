//
// C++ Implementation: mutex_sdl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "mutex_sdl.h"
#ifdef SDL_ENABLED
#include "error_macros.h"
#include "os/memory.h"

void Mutex_SDL::lock() {

	SDL_LockMutex(mutex);
}
void Mutex_SDL::unlock() {

	SDL_UnlockMutex(mutex);

}
bool Mutex_SDL::try_lock() {

	ERR_PRINT("SDL_mutex can't try_lock()!");
	return false;
}


Mutex* Mutex_SDL::create_func_SDL() {

	return memnew( Mutex_SDL );
}

void Mutex_SDL::set_as_default() {

	create_func=create_func_SDL;
}

Mutex_SDL::Mutex_SDL() {

	mutex = SDL_CreateMutex();
}


Mutex_SDL::~Mutex_SDL() {

	SDL_DestroyMutex(mutex);
}

#endif
