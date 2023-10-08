//
// C++ Interface: thread_safe
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef THREAD_SAFE_H
#define THREAD_SAFE_H


#include "os/mutex.h"

class ThreadSafe {

	Mutex *mutex;
public:

	inline void lock() const { if (mutex) mutex->lock(); }
	inline void unlock() const { if (mutex) mutex->unlock(); }

	ThreadSafe() { 
		mutex = Mutex::create(); 
		if (!mutex) {
			
			WARN_PRINT("THREAD_SAFE defined, but no default mutex type");
		}
	}
	~ThreadSafe() { if (mutex) memdelete( mutex ); }
	
};


class ThreadSafeMethod {

	const ThreadSafe *_ts;
public:
	ThreadSafeMethod(const ThreadSafe *p_ts) {
	
		_ts=p_ts;
		_ts->lock();
	}
	
	~ThreadSafeMethod() { _ts->unlock(); }
};


#ifndef NO_THREAD_SAFE

#define _THREAD_SAFE_CLASS_ ThreadSafe __thread__safe__;
#define _THREAD_SAFE_METHOD_  ThreadSafeMethod __thread_safe_method__(&__thread__safe__);
#define _THREAD_SAFE_LOCK_ __thread__safe__.lock()
#define _THREAD_SAFE_UNLOCK_ __thread__safe__.unlock()

#endif




#endif
