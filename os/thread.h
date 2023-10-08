//
// C++ Interface: thread
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef THREAD_H
#define THREAD_H

#include "signals/signals.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/

class Thread {
public:
			
	enum Priority {
		
		PRIORITY_HIGH,
  		PRIORITY_NORMAL,
	  	PRIORITY_LOW	
	};
	
	struct Settings {
		
		Priority priority;
		Settings() { priority=PRIORITY_NORMAL; }
	};
	
protected:	
	static Thread* (*create_func)(Method,const Settings&);
	static Uint32 (*get_thread_ID_func)();
	static void (*wait_to_finish_func)(Thread*);
public:
		
	
	static Uint32 get_thread_ID(); ///< get the ID of the caller thread
	static void wait_to_finish(Thread *p_thread); ///< waits until thread is finished, and deallocates it.
	static Thread * create(Method p_callback,const Settings& p_settings=Settings()); ///< Static function to create a thread, will call p_callback
	
	Thread();
	virtual ~Thread();

};

#endif
