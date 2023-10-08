#ifdef POSTGRES_ENABLED
//
// C++ Interface: sql_client_postgres
//
// Description: 
//
//
// Author: Tomas Neme <lacrymology@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SQL_CLIENT_POSTGRES_H
#define SQL_CLIENT_POSTGRES_H

#include <postgresql/libpq-fe.h>
#include "fileio/sql_client.h"

/**
	@author Tomas Neme <lacrymology@gmail.com>
*/
class SQL_Client_Postgres : public SQL_Client {	

private:
	PGresult *result;
	int cursor_position;
	PGconn *db;
	String db_path;

	virtual void connect();

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
	
	SQL_Client_Postgres(String p_db_path);
	~SQL_Client_Postgres();
	
};

#endif

#endif

