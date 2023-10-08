//
// C++ Implementation: iobj
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "iobj.h"
#include "error_macros.h"

String IObj::persist_as() const {
	
	return get_type();
}


String IObj::get_type() const { return "IObj"; }

bool IObj::is_type(String p_type) const { 
	return (p_type=="IObj");
}

void IObj::init() {
	
	
}

void IObj::predelete() {
	
	
}

IObj::IObj() {
	_ishared=NULL;
}


IObj::~IObj()
{
}

IObjOwner * IObjOwner::singleton=NULL;
	
IObjOwner *IObjOwner::get_singleton() {
	
	return singleton;
}


void IObjOwner::lock() {
	
	if (get_singleton())
		get_singleton()->_lock();	
}
void IObjOwner::unlock() {
	
	if (get_singleton())
		get_singleton()->_unlock();	
}


IObjOwner::IObjOwner() {
	
	ERR_FAIL_COND(singleton && singleton!=this);
	singleton=this;
}

IObjOwner::~IObjOwner() {
	
	singleton=NULL;
}

