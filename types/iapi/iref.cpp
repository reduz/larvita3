//
// C++ Implementation: iref
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "iref.h"

IObjShared *IRefBase::allocate_ishared( IObj *p_iobj ) {
	
	ERR_FAIL_COND_V(IObjOwner::get_singleton()==NULL,NULL);
	return IObjOwner::get_singleton()->allocate_ishared(p_iobj);	
}

void IRefBase::deallocate_ishared( IObjShared *p_ishared ) {
		
	ERR_FAIL_COND(IObjOwner::get_singleton()==NULL);
	IObj* obj = p_ishared->obj;
	IObjOwner::get_singleton()->deallocate_ishared( p_ishared );
	obj->_ishared = NULL;
}

