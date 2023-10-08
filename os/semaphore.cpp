//
// C++ Implementation: semaphore
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "semaphore.h"
#include "error_macros.h"

Semaphore* (*Semaphore::create_func)()=0;

Semaphore *Semaphore::create() {

	ERR_FAIL_COND_V( !create_func, 0 );

	return create_func();
}


Semaphore::~Semaphore() {

}