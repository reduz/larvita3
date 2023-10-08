
#ifndef MUTEX_H
#define MUTEX_H



/**
 * @class Mutex 
 * @author Juan Linietsky
 * Portable Mutex (thread-safe locking) implementation.
 * Mutexes are always recursive ( they don't self-lock in a single thread ).
 * Mutexes can be used with a Lockp object like this, to avoid having to worry about unlocking:
 * Lockp( mutex );
 */


class Mutex {
protected:	
	static Mutex* (*create_func)();
	
public:

	virtual void lock()=0; ///< Lock the mutex, block if locked by someone else
	virtual void unlock()=0; ///< Unlock the mutex, let other threads continue
	virtual bool try_lock()=0; ///< Attempt to lock the mutex, true on success, false means it can't lock.

	static Mutex * create(); ///< Create a mutex
	
	virtual ~Mutex();
};


#endif
