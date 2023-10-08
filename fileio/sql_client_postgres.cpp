#ifdef POSTGRES_ENABLED
//
// C++ Implementation: sql_client_postgres
//
// Description:
//
//
// Author: Tomas Neme <lacrymology@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sql_client_postgres.h"


String SQL_Client_Postgres::get_custom_string(CustomString p_string) {

	switch(p_string) {

		case CUSTOM_CURRENT_TIMESTAMP: return "now";
	}

	return "";
}

void SQL_Client_Postgres::transaction_begin() {

	this->query("BEGIN TRANSACTION;");
}
void SQL_Client_Postgres::transaction_end() {

	this->query("END TRANSACTION;");

}
void SQL_Client_Postgres::transaction_cancel() {

	this->query("ROLLBACK TRANSACTION;");

}

void SQL_Client_Postgres::connect()
{
	this->db = PQconnectdb(this->db_path.utf8().get_data());
	// set the error handlers
}

Error SQL_Client_Postgres::query(String p_sql) {
	//printf("SQL_Client_Postgres::query(%s)\n",p_sql.utf8().get_data());

	if (result) {

		PQclear ( result );
		result=NULL;
	}

	if (!db) {
		this->connect();
	}

	ERR_FAIL_COND_V(db == 0,ERR_CANT_OPEN);

	p_sql.strip_edges();

	this->result = PQexec(db,p_sql.utf8().get_data());

	switch(PQresultStatus(this->result))
	{
		case PGRES_EMPTY_QUERY:
		case PGRES_COMMAND_OK:
		case PGRES_TUPLES_OK:
			//printf("query ok: %d\n",PQresultStatus(this->result));
			// it worked
			break;

		default:
			// error
			//printf("query error: %d\n",PQresultStatus(this->result));
			this->cursor_position = -1;
			ERR_PRINT(PQresultErrorMessage(this->result));
			return ERROR_QUERY_FAILED;
			break;
	}

	this->cursor_position = 0;
	return OK;

}

int SQL_Client_Postgres::get_last_insert_ID(String p_table, String p_column) const
{
	//printf("SQL_Client_Postgres::get_last_insert_ID(%s,%s)\n",p_table.utf8().get_data(),p_column.utf8().get_data());
	String query = "SELECT currval(\'";
	query += p_table + "_" + p_column + "_seq\')";
	//printf("executing %s\n",query.utf8().get_data());
	PGresult* currval = PQexec(this->db,query.utf8().get_data());
	//printf("last id: %s\n",PQgetvalue(currval,0,0));
	if(PQresultStatus(currval) == PGRES_TUPLES_OK)
	{
		String res = PQgetvalue(currval,0,0);
		return res.to_int();
	}
	else
		return -1;
}

bool SQL_Client_Postgres::cursor_is_at_end()
{
	//printf("SQL_Client_Postgres::cursor_is_at_end()\n");
	return this->cursor_position >= PQntuples(this->result);
}

bool SQL_Client_Postgres::cursor_next() {
	//printf("SQL_Client_Postgres::cursor_next()");

	ERR_FAIL_COND_V( !this->db,false );

	if (!this->result)
		return false;
	if(this->cursor_position < 0)
		return false;
	if(this->cursor_is_at_end())
		return false;

	this->cursor_position++;
	return !this->cursor_is_at_end();


}

/* defining this shit here because there's no way to include it from postgres */
#define BOOLOID			16
#define BYTEAOID		17
#define CHAROID			18
#define TEXTOID			25
#define BPCHAROID		1042
#define VARCHAROID		1043
#define INT8OID			20
#define INT2OID			21
#define INT2VECTOROID	22
#define INT4OID			23
#define NUMERICOID		1700
#define FLOAT4OID 700
#define FLOAT8OID 701


//sqlite3_column_name(statement, i)
Error SQL_Client_Postgres::cursor_get_column(int p_column,Variant& p_field) {
	//printf("SQL_Client_Postgres::cursor_get_column(%d)\n",p_column);

	p_field=Variant();

	ERR_FAIL_COND_V( !this->db,ERR_UNCONFIGURED );
	ERR_FAIL_COND_V( !this->result, ERR_UNCONFIGURED );
	ERR_FAIL_INDEX_V( p_column, PQnfields(this->result), ERR_INVALID_PARAMETER );

	switch(PQftype(this->result,p_column))
	{
		case BOOLOID: {
			// boolean: 'true'/'false'
			String val = PQgetvalue(this->result,this->cursor_position,p_column);
			//printf("BOOL (%s)\n",val.utf8().get_data());
			if( val == "true" )
				p_field = true;
			else
				p_field = false;
			} break;

		case BYTEAOID: {
			char *val = PQgetvalue(this->result,this->cursor_position,p_column);
			//printf("BYTEA (%s)\n",val);
			size_t len;
			unsigned char *un_val = PQunescapeBytea((unsigned char*)val, &len);
			//printf("len: %d\n",len);
			p_field.create(Variant::BYTE_ARRAY,len);

			for(int i = 0 ; i < (int)len ; i++)
			{
				//printf("%d",un_val[i]);
				p_field.array_set_byte(i,un_val[i]);
			}
			//printf("\n");

			PQfreemem(un_val);

			} break;

		case CHAROID:
		case TEXTOID:
		case BPCHAROID:
			// char(length), non-blank-padded string, variable sotrage length
		case VARCHAROID:
			// varchar(length), non-blank-padded string, variable storage length
			{
			String val = PQgetvalue(this->result, this->cursor_position, p_column);
			//printf("STRING (%s)\n",val.utf8().get_data());
			p_field = val;
			} break;

		case INT8OID:
		case INT2OID:
		case INT4OID: {
			String val = PQgetvalue(this->result, this->cursor_position, p_column);
			//printf("INT (%s)\n",val.utf8().get_data());
			p_field = val.to_int();
			} break;

		case FLOAT4OID:
		case NUMERICOID:
			// numeric(precision, decimal), arbitrary precision number"
		case FLOAT8OID: {
			String val = PQgetvalue(this->result, this->cursor_position, p_column);
			//printf("FLOAT (%s)\n",val.utf8().get_data());
			p_field = val.to_double();
			} break;
	}

	return OK;
}

Variant::Type SQL_Client_Postgres::cursor_get_column_type(int p_column) {
	//printf("cursor_get_column_type\n");

	ERR_FAIL_COND_V( !this->result, Variant::NIL );
	ERR_FAIL_INDEX_V( p_column, PQnfields(this->result), Variant::NIL );

	switch( PQftype(this->result, p_column) )
	{
		case BOOLOID:
			//printf("bool\n");
			return Variant::BOOL;
		case BYTEAOID:
			//printf("BYTE_ARRAY\n");
			return Variant::BYTE_ARRAY;
		case CHAROID:
		case TEXTOID:
		case BPCHAROID:
			// char(length), non-blank-padded string, variable sotrage length
		case VARCHAROID:
			// varchar(length), non-blank-padded string, variable storage length
			//printf("STRING\n");
			return Variant::STRING;
		case INT8OID:
		case INT2OID:
		case INT4OID:
			//printf("INT\n");
			return Variant::INT;
		case FLOAT4OID:
		case NUMERICOID:
			// numeric(precision, decimal), arbitrary precision number"
		case FLOAT8OID:
			//printf("REAL\n");
			return Variant::REAL;

		default:
			break;
	}
	return Variant::NIL;
}

String SQL_Client_Postgres::cursor_get_column_name(int p_column) {

	ERR_FAIL_COND_V( !this->result, "" );
	ERR_FAIL_INDEX_V( p_column, PQnfields(this->result), "" );

	return PQfname( this->result, p_column );

}

void SQL_Client_Postgres::cursor_close() {
	//printf("SQL_Client_Postgres::cursor_close\n");

	ERR_FAIL_COND( !this->db );
	if (!this->result)
		return;
	PQclear ( this->result );
	this->result = NULL;
}

String SQL_Client_Postgres::make_blob(const Uint8* p_data,int p_size) {

	String blob_text;
	blob_text="E'";
	size_t new_size;
	unsigned char* val = PQescapeByteaConn(this->db,p_data,p_size,&new_size);
	blob_text += String((char*) val) + "\'";
	PQfreemem(val);
	return blob_text;

}

SQL_Client_Postgres::SQL_Client_Postgres(String p_db_path) {

	this->db=NULL;
	this->db_path=p_db_path;
	this->cursor_position=-1;
	this->result=NULL;
}


SQL_Client_Postgres::~SQL_Client_Postgres() {

	if (this->result) {

		this->cursor_close();
	}

	if (db) {
		PQfinish(this->db);
		this->db=NULL;
	}

}


#endif

