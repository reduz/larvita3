//
// C++ Implementation: iapi_db
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "iapi_db.h"

IAPI_DB * IAPI_DB::singleton=NULL;


IAPI_DB * IAPI_DB::get_singleton() {
	
	return singleton;
}

void IAPI_DB::iapi_set_objid(IAPIRef p_iapi,ObjID p_ID) {

	p_iapi->set_ID(p_ID);
}

IAPI_DB::IAPI_DB() {
	
	singleton=this;
}


IAPI_DB::~IAPI_DB() {
}


