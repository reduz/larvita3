//
// C++ Interface: iapi_db_file
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IAPI_DB_FILE_H
#define IAPI_DB_FILE_H


#include "iapi_db.h"
#include "fileio/sql_client.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class IAPI_DB_File : public IAPI_DB {
	
	
	String db_path;	
	Table<String,int> name_cache;
	Table<int,String> id_cache;
	int max_id;

	ObjID get_new_id();
	
public:
	
	virtual Error save(IAPIRef p_iapi,String p_path=""); 
	virtual Error load(IAPIRef p_iapi,ObjID p_ID);
	
	virtual Error get_type(ObjID p_ID, String &p_type);
	virtual ObjID get_ID(String p_path);
	virtual String get_path(ObjID p_ID);
	virtual Error set_path(ObjID p_ID,String p_path);
	virtual Error get_creation_params(ObjID p_ID,IAPI::CreationParams &p_params);
		
		
	virtual Error get_listing(List<ListingElement>* p_listing, ListingMode p_mode,String p_string="",bool p_exact_match=true);
	
	IAPI_DB_File(String p_path);
	~IAPI_DB_File();

};

#endif
