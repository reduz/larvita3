//
// C++ Implementation: thread
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "thread.h"

Thread* (*Thread::create_func)(Method,const Settings&)=NULL;
Uint32 (*Thread::get_thread_ID_func)()=NULL;
void (*Thread::wait_to_finish_func)(Thread*)=NULL;

Uint32 Thread::get_thread_ID() {
	
	if (get_thread_ID_func)
		return get_thread_ID_func();
	return 0;
}

Thread* Thread::create(Method p_callback,const Settings& p_settings) {
	
	if (create_func) {
		 
		return create_func(p_callback,p_settings);
	}
	return NULL;
}

void Thread::wait_to_finish(Thread *p_thread) {
	
	if (wait_to_finish_func)
		wait_to_finish_func(p_thread);
		
}

Thread::Thread()
{
}


Thread::~Thread()
{
}


