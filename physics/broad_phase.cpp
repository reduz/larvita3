//
// C++ Implementation: broad_phase
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "broad_phase.h"

BroadPhase* (*BroadPhase::create_func)()=NULL;

BroadPhase* BroadPhase::create() {
		
	ERR_FAIL_COND_V(!create_func,NULL);
	return create_func();
}

BroadPhase::BroadPhase()
{
}


BroadPhase::~BroadPhase()
{
}


