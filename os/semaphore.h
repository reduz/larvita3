//
// C++ Interface: semaphore
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "error_list.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class Semaphore {
protected:	
	static Semaphore* (*create_func)();
	
public:

	virtual Error wait()=0; ///< wait until semaphore has positive value
	virtual Error try_wait()=0; ///< same as wait, but return OK if clear, or ERR_TIMEOUT if would have suspended
	virtual Error wait_with_timeout(int p_msec)=0; ///< same as try_wait, but with a timeout
	virtual Error post()=0; ///< unlock the semaphore, incrementing the    value
	virtual int get() const=0; ///< get semaphore value
	
	static Semaphore * create(); ///< Create a mutex
	
	virtual ~Semaphore();
};


#endif
