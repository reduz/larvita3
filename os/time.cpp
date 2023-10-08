//
// C++ Implementation: time
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "time.h"

OSTime* OSTime::singleton=NULL;

OSTime* OSTime::get_singleton() {

	return singleton;
}


OSTime::OSTime() {

	singleton=this;
}


OSTime::~OSTime()
{
}


