//
// C++ Implementation: thread_sdl
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "thread_sdl.h"
#ifdef SDL_ENABLED


int Thread_SDL::_initiate_thread(void* p_thread) {

	Thread_SDL *t=(Thread_SDL*)p_thread;
	t->call_method.call();
	return 0;
}



Thread* Thread_SDL::create_func_SDL(Method p_method,const Settings&) {

	Thread_SDL * t = memnew( Thread_SDL );
	t->call_method=p_method;
	t->thread = SDL_CreateThread(_initiate_thread,t);
	return t;
}
Uint32 Thread_SDL::get_thread_ID_func_SDL() {

	return SDL_ThreadID();
}
void Thread_SDL::wait_to_finish_func_SDL(Thread* p_thread) {

	int st;
	Thread_SDL *t = SAFE_CAST<Thread_SDL*>(p_thread);
	ERR_FAIL_COND(!t);
	SDL_WaitThread(t->thread, &st);
}

void Thread_SDL::set_as_default() {

	create_func=create_func_SDL;
	get_thread_ID_func=get_thread_ID_func_SDL;
	wait_to_finish_func=wait_to_finish_func_SDL;
}

Thread_SDL::Thread_SDL() {

	thread=NULL;
}


Thread_SDL::~Thread_SDL()
{
}

#endif
