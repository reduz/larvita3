//
// C++ Implementation: memory_static
//
// Description: 
//
//
// Author: Juan Linietsky <red@lunatea>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "memory_pool_static.h"

MemoryPoolStatic *MemoryPoolStatic::singleton=0;

MemoryPoolStatic *MemoryPoolStatic::get_singleton() {
	
	return singleton;
}


MemoryPoolStatic::MemoryPoolStatic() {
	
	singleton=this;
}


MemoryPoolStatic::~MemoryPoolStatic()
{
}


