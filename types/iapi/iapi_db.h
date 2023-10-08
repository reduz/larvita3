//
// C++ Interface: iapi_db
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IAPI_DB_H
#define IAPI_DB_H

#include "iobj.h"
#include "iapi.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class IAPI_DB {
protected:

		
	static IAPI_DB * singleton;
	void iapi_set_objid(IAPIRef p_iapi,ObjID p_ID);
public:

	static IAPI_DB * get_singleton();
	virtual Error save(IAPIRef p_iapi,String p_path="")=0; 
	virtual Error load(IAPIRef p_iapi,ObjID p_ID)=0;
	
	virtual Error get_type(ObjID p_ID, String &p_type)=0;
	virtual ObjID get_ID(String p_path)=0;
	virtual String get_path(ObjID p_ID)=0;
	virtual Error set_path(ObjID p_ID,String p_path)=0;
	virtual Error get_creation_params(ObjID p_ID,IAPI::CreationParams &p_params)=0;
		
	enum ListingMode {
		LISTING_PATH,
  		LISTING_TYPE
	};
			
	struct ListingElement {
		
		ObjID id;
		String path;
		String type;
	};
		
	virtual Error get_listing(List<ListingElement>* p_listing, ListingMode p_mode,String p_string="",bool p_exact_match=true)=0;
			
	IAPI_DB();
	virtual ~IAPI_DB();
};

#endif
