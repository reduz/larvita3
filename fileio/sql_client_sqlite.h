//
// C++ Interface: sql_client_sqlite
//
// Description: 
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SQL_CLIENT_SQLITE_H
#define SQL_CLIENT_SQLITE_H

#include "fileio/sqlite3/sqlite3.h"
#include "fileio/sql_client.h"

/**
	@author Juan Linietsky <reduzio@gmail.com>
*/
class SQL_Client_SQLite : public SQL_Client {	
	
	sqlite3_stmt *statement;
	int last_insert_id;
	sqlite3 *db;
	String db_path;
public:
	
	virtual Error query(String p_sql);
	virtual int get_last_insert_ID(String p_table, String p_column) const;
	
	virtual bool cursor_next(); /// false if OK, true when no more rows
	virtual String cursor_get_column_name(int p_column);
	virtual Variant::Type cursor_get_column_type(int p_column);
	virtual Error cursor_get_column(int p_column,Variant& p_field);
	virtual void cursor_close();
	virtual bool cursor_is_at_end();
	
	virtual String get_custom_string(CustomString);
	virtual String make_blob(const Uint8* p_data,int p_size);
	
	virtual void transaction_begin();
	virtual void transaction_end();
	virtual void transaction_cancel();
	
	static bool test(String p_db_path);
	
	SQL_Client_SQLite(String p_db_path);
	~SQL_Client_SQLite();
	
};

#endif
