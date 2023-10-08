//
// C++ Interface: iapi_db_sql
//
// Description: 
//
//
// Author: ,,, <red@lunatea>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef IAPI_DB_SQL_H
#define IAPI_DB_SQL_H

#include "iapi_db.h"
#include "fileio/sql_client.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class IAPI_DB_SQL : public IAPI_DB {

	SQL_Client *sql_client;
	
	String validate_path_syntax(String p_path);
	String validate_insert_path(ObjID p_ID,String p_path);

public:

	virtual Error save(IAPIRef p_iapi,String p_path=""); 
	virtual Error load(IAPIRef p_iapi,ObjID p_ID);
	
	virtual Error get_type(ObjID p_ID, String &p_type);
	virtual ObjID get_ID(String p_path);
	virtual String get_path(ObjID p_ID);
	virtual Error set_path(ObjID p_ID,String p_path);
	virtual Error get_creation_params(ObjID p_ID,IAPI::CreationParams &p_params);
		
		
	virtual Error get_listing(List<ListingElement>* p_listing, ListingMode p_mode,String p_string="",bool p_exact_match=true);
	
	IAPI_DB_SQL(SQL_Client *p_sql_client);
	~IAPI_DB_SQL();

};

#endif
