//
// C++ Interface: sql_client
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SQL_CLIENT_H
#define SQL_CLIENT_H

#include "error_list.h"
#include "rstring.h"
#include "variant.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class SQL_Client {
public:
	
	enum CustomString {
		CUSTOM_CURRENT_TIMESTAMP
	};
	
	
	virtual Error query(String p_sql)=0;
	virtual int get_last_insert_ID(String p_table, String p_column) const=0;
	
	virtual bool cursor_next()=0; /// false if OK, true when no more rows
	virtual String cursor_get_column_name(int p_column)=0;
	virtual Variant::Type cursor_get_column_type(int p_column)=0;
	virtual Error cursor_get_column(int p_column,Variant& p_field)=0;
	virtual bool cursor_is_at_end()=0;
	virtual void cursor_close()=0;
	
	virtual String get_custom_string(CustomString)=0;
	virtual String make_blob(const Uint8* p_data,int p_size)=0;
	
	virtual void transaction_begin()=0;
	virtual void transaction_end()=0;
	virtual void transaction_cancel()=0;
	
	SQL_Client();
	virtual ~SQL_Client();

};

#endif
