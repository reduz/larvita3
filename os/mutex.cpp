//
// C++ Implementation: mutex
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "mutex.h"
#include "error_macros.h"

Mutex* (*Mutex::create_func)()=0;

Mutex *Mutex::create() {

	ERR_FAIL_COND_V( !create_func, 0 );

	return create_func();
}


Mutex::~Mutex() {


}