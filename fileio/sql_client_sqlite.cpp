//
// C++ Implementation: sql_client_sqlite
//
// Description:
//
//
// Author: Juan Linietsky <reduzio@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sql_client_sqlite.h"

#include "types/global_vars.h"

String SQL_Client_SQLite::get_custom_string(CustomString p_string) {

	switch(p_string) {

		case CUSTOM_CURRENT_TIMESTAMP: return "current_timestamp";
	}

	return "";
}

void SQL_Client_SQLite::transaction_begin() {
	
//	query("BEGIN TRANSACTION;");
}
void SQL_Client_SQLite::transaction_end() {
	
//	query("END TRANSACTION;");
	
}
void SQL_Client_SQLite::transaction_cancel() {
	
//	query("ROLLBACK TRANSACTION;");

}


Error SQL_Client_SQLite::query(String p_sql) {

	if (statement) {

		sqlite3_finalize ( statement );
		statement=NULL;
	}

	if (db) {
		sqlite3_close(db);
		db=NULL;
	}

	//printf("query is %s\ndb_path %s\n",p_sql.utf8().get_data(),db_path.utf8().get_data());
	
	int rc = sqlite3_open(db_path.utf8().get_data(), &db);

	ERR_FAIL_COND_V(rc!=0,ERR_CANT_OPEN);

	p_sql.strip_edges();

	if (p_sql.findn("select")==0) {
		/* for SELECT queries */
		const char  *rest;
		int rc = sqlite3_prepare ( db, p_sql.utf8().get_data(), -1,
			&statement, &rest );

		if (rc!=SQLITE_OK) {
			
			sqlite3_finalize ( statement );
			sqlite3_close(db);
			db=NULL;
			ERR_FAIL_COND_V(rc!=SQLITE_OK,ERROR_QUERY_FAILED);
		}

		cursor_next();

	} else {
		/* for SELECT queries */

		char *error_msg;
		rc = sqlite3_exec(db, p_sql.utf8().get_data(), NULL, NULL, &error_msg);
		printf("query size is %i\n", p_sql.size());
		if (rc!=SQLITE_OK) {

			sqlite3_close(db);
			db=NULL;
			ERR_PRINT(error_msg);
			ERR_FAIL_COND_V(rc!=SQLITE_OK,ERROR_QUERY_FAILED);
		} else {

			if (p_sql.findn("insert")==0) {

				last_insert_id=(int)sqlite3_last_insert_rowid(db);
			}
			sqlite3_close(db);
			db = NULL;
		}

	}


	return OK;

}

int SQL_Client_SQLite::get_last_insert_ID(String,String) const {

	return last_insert_id;
}


bool SQL_Client_SQLite::cursor_is_at_end() {
	
	return statement==NULL;	
}

bool SQL_Client_SQLite::cursor_next() {

	ERR_FAIL_COND_V( !db,false );
	if (!statement)
		return false;

	int rc;
	int max_attempts=20;
	int sleep_ms=1;

	do {

		rc = sqlite3_step(statement);
		max_attempts--;
		if (rc==SQLITE_BUSY) {
			sqlite3_sleep(sleep_ms);
		}
	} while (rc==SQLITE_BUSY && max_attempts>0);
	
	ERR_FAIL_COND_V(max_attempts==0,false);
	
	if (rc==SQLITE_DONE) {

		sqlite3_finalize ( statement );
		statement=NULL;
		sqlite3_close(db);
		return false;
	}

	// rc is then SQLITE_ROW

	return true;


}

//sqlite3_column_name(statement, i)
Error SQL_Client_SQLite::cursor_get_column(int p_column,Variant& p_field) {

	p_field=Variant();

	ERR_FAIL_COND_V( !db,ERR_UNCONFIGURED );
	ERR_FAIL_COND_V( !statement, ERR_UNCONFIGURED );
	ERR_FAIL_INDEX_V( p_column, sqlite3_column_count(statement), ERR_INVALID_PARAMETER );

	switch( sqlite3_column_type(statement, p_column) ) {

		case SQLITE_FLOAT: {

			p_field = sqlite3_column_double( statement, p_column );
		} break;
		case SQLITE_INTEGER: {

			p_field = sqlite3_column_int( statement, p_column );
		} break;
		case SQLITE_TEXT: {

			p_field = String((const char*)sqlite3_column_text( statement, p_column ));
		} break;
		case SQLITE_BLOB: {
			int len = sqlite3_column_bytes(statement, p_column);
                        const Uint8* data = (const Uint8*)sqlite3_column_blob( statement, p_column );
			p_field.create( Variant::BYTE_ARRAY, len );
			//printf("blob len is %i\n",len);

			for (int i=0;i<len;i++)
				p_field.array_set_byte( i, data[i] );
		} break;
		case SQLITE_NULL: {
			p_field=Variant();
		} break;
	}

	return OK;
}

Variant::Type SQL_Client_SQLite::cursor_get_column_type(int p_column) {

	ERR_FAIL_COND_V( !statement, Variant::NIL );
	ERR_FAIL_INDEX_V( p_column, sqlite3_column_count(statement), Variant::NIL );


	switch( sqlite3_column_type(statement, p_column) ) {

		case SQLITE_FLOAT: {

			return Variant::REAL;
		} break;
		case SQLITE_INTEGER: {

			return Variant::INT;
		} break;
		case SQLITE_TEXT: {

			return Variant::STRING;
		} break;
		case SQLITE_BLOB: {

			return Variant::BYTE_ARRAY;
		} break;
		default:
		{}
	}

	return Variant::NIL;
}

String SQL_Client_SQLite::cursor_get_column_name(int p_column) {

	ERR_FAIL_COND_V( !statement, "" );
	ERR_FAIL_INDEX_V( p_column, sqlite3_column_count(statement), "" );

	return sqlite3_column_name( statement, p_column );

}

void SQL_Client_SQLite::cursor_close() {

	ERR_FAIL_COND( !db );
	if (!statement)
		return;
	sqlite3_finalize ( statement );
	statement=NULL;
}

String SQL_Client_SQLite::make_blob(const Uint8* p_data,int p_size) {

	
	String blob_text;
	blob_text="x'";
	for (int i=0;i<p_size;i++) {
		
		static const char hextable[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
		char hexstr[3]={0,0,0};
		hexstr[0]=hextable[p_data[i]>>4];
		hexstr[1]=hextable[p_data[i]&0xF];
		blob_text+=hexstr;		
	}
	blob_text+="'";
	return blob_text;	

}

bool SQL_Client_SQLite::test(String p_db_path) {

	sqlite3 *db;
	int rc = sqlite3_open(p_db_path.utf8().get_data(), &db);
	printf("opening db %s, result %i\n",p_db_path.ascii().get_data(),rc);

	if (rc!=0) {
	
		return false;
	} else {
	
		sqlite3_close(db);
		return true;
	}

}

SQL_Client_SQLite::SQL_Client_SQLite(String p_db_path) {

	db=NULL;
	db_path=GlobalVars::get_singleton()->translate_path(p_db_path);
	last_insert_id=-1;
	statement=NULL;
}


SQL_Client_SQLite::~SQL_Client_SQLite() {

	if (statement) {

		sqlite3_finalize ( statement );
		statement=NULL;
	}

	if (db) {
		sqlite3_close(db);
		db=NULL;
	}

}


